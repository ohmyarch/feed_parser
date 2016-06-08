/****************************************************************************
**
** Copyright (C) 2016 Michael Yang
** Contact: ohmyarchlinux@gmail.com
**
** This file is part of the feed_parser.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include <boost/property_tree/xml_parser.hpp>
#include <feed/atom_parser.h>

class atom_exception : public std::exception {
  public:
    const char *what() const noexcept override { return ""; }
};

namespace feed {
boost::optional<atom_data> atom_parser::parse(const std::string &uri) {
    web::http::client::http_client client(
        utility::conversions::to_string_t(uri), http_client_config_);
    const auto response = client.request(web::http::methods::GET);

    boost::property_tree::ptree root;

    atom_data data;

    try {
        // FIXME
        utility::istringstream_t stream(
            response.get().extract_string(true).get());

        boost::property_tree::read_xml(stream, root);

        const auto &feed_node = root.get_child("feed");

        data.id_ = feed_node.get<std::string>("id");

        const auto &title_node = feed_node.get_child("title");
        data.title_.value_ = title_node.get_value<std::string>();
        const auto title_type_node =
            title_node.get_child_optional("<xmlattr>.type");
        if (title_type_node) {
            std::string type = title_type_node->get_value<std::string>();
            if (type == "html")
                data.title_.type_ = text::type::html;
            else if (type == "xhtml")
                data.title_.type_ = text::type::xhtml;
        }

        const auto generator_node = feed_node.get_child_optional("generator");
        if (generator_node)
            data.generator_.emplace(
                generator_node->get_value<std::string>(),
                generator_node->get_optional<std::string>("<xmlattr>.uri"),
                generator_node->get_optional<std::string>("<xmlattr>.version"));

        data.icon_ = feed_node.get_optional<std::string>("icon");
        data.logo_ = feed_node.get_optional<std::string>("logo");

        const auto rights_node = feed_node.get_child_optional("rights");
        if (rights_node) {
            std::string rights = rights_node->get_value<std::string>();
            enum text::type type = text::type::text;
            const auto rights_type_node =
                rights_node->get_child_optional("<xmlattr>.type");
            if (rights_type_node) {
                std::string type_str =
                    rights_type_node->get_value<std::string>();
                if (type_str == "html")
                    type = text::type::html;
                else if (type_str == "xhtml")
                    type = text::type::xhtml;
            }

            data.rights_.emplace(std::move(rights), type);
        }

        const auto subtitle_node = feed_node.get_child_optional("subtitle");
        if (subtitle_node) {
            std::string subtitle = subtitle_node->get_value<std::string>();
            enum text::type type = text::type::text;
            const auto subtitle_type_node =
                subtitle_node->get_child_optional("<xmlattr>.type");
            if (subtitle_type_node) {
                std::string type_str =
                    subtitle_type_node->get_value<std::string>();
                if (type_str == "html")
                    type = text::type::html;
                else if (type_str == "xhtml")
                    type = text::type::xhtml;
            }

            data.subtitle_.emplace(std::move(subtitle), type);
        }

        return std::move(data);
    } catch (...) {
        std::cerr << "Error!" << std::endl;
    }

    return {};
}
}