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
        data.title_ = feed_node.get<std::string>("title");

        const auto generator_node = feed_node.get_child_optional("generator");
        if (generator_node)
            data.generator_.emplace(
                generator_node->get_value<std::string>(),
                generator_node->get_optional<std::string>("<xmlattr>.uri"),
                generator_node->get_optional<std::string>("<xmlattr>.version"));

        return std::move(data);
    } catch (...) {
        std::cerr << "Error!" << std::endl;
    }

    return {};
}
}
