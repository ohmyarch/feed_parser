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
namespace atom {
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

            data.rights_.emplace(rights_node->get_value<std::string>(), type);
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

        std::vector<person> authors;
        std::vector<link> links;
        std::vector<category> categories;
        std::vector<person> contributors;

        for (const auto &feed_child : feed_node)
            if (feed_child.first == "author") {
                const auto &author_node = feed_child.second;

                authors.emplace_back(
                    author_node.get<std::string>("name"),
                    author_node.get_optional<std::string>("email"),
                    author_node.get_optional<std::string>("uri"));
            } else if (feed_child.first == "link") {
                const auto &link_attr_node =
                    feed_child.second.get_child("<xmlattr>");

                link link;
                link.href_ = link_attr_node.get<std::string>("href");
                link.href_lang_ =
                    link_attr_node.get_optional<std::string>("hreflang");
                link.length_ =
                    link_attr_node.get_optional<std::uint64_t>("length");
                link.title_ = link_attr_node.get_optional<std::string>("title");
                link.type_ = link_attr_node.get_optional<std::string>("type");

                const auto rel_node = link_attr_node.get_child_optional("rel");
                if (rel_node) {
                    const std::string ref = rel_node->get_value<std::string>();
                    if (ref == "alternate")
                        link.rel_ = rel::alternate;
                    else if (ref == "enclosure")
                        link.rel_ = rel::enclosure;
                    else if (ref == "related")
                        link.rel_ = rel::related;
                    else if (ref == "self")
                        link.rel_ = rel::self;
                    else
                        link.rel_ = rel::via;
                }

                links.emplace_back(std::move(link));
            } else if (feed_child.first == "category") {
                const auto &category_attr_node =
                    feed_child.second.get_child("<xmlattr>");

                categories.emplace_back(
                    category_attr_node.get<std::string>("term"),
                    category_attr_node.get_optional<std::string>("scheme"),
                    category_attr_node.get_optional<std::string>("label"));
            } else if (feed_child.first == "contributor") {
                const auto &contributor_node = feed_child.second;

                contributors.emplace_back(
                    contributor_node.get<std::string>("name"),
                    contributor_node.get_optional<std::string>("email"),
                    contributor_node.get_optional<std::string>("uri"));
            } else if (feed_child.first == "entry") {
                const auto &entry_node = feed_child.second;

                entry entry;
                entry.id_ = entry_node.get<std::string>("id");

                const auto &title_node = entry_node.get_child("title");
                entry.title_.value_ = title_node.get_value<std::string>();
                const auto title_type_node =
                    title_node.get_child_optional("<xmlattr>.type");
                if (title_type_node) {
                    std::string type =
                        title_type_node->get_value<std::string>();
                    if (type == "html")
                        entry.title_.type_ = text::type::html;
                    else if (type == "xhtml")
                        entry.title_.type_ = text::type::xhtml;
                }

                const auto content_node =
                    entry_node.get_child_optional("content");
                if (content_node) {
                    std::string content =
                        content_node->get_value<std::string>();
                    const auto content_type_node =
                        content_node->get_child_optional("<xmlattr>.type");
                    if (content_type_node) {
                        std::string type =
                            content_type_node->get_value<std::string>();
                        if (type == "html")
                            entry.content_.emplace(std::move(content),
                                                   text::type::html);
                        else if (type == "xhtml")
                            entry.content_.emplace(std::move(content),
                                                   text::type::xhtml);
                        else
                            entry.content_.emplace(std::move(content));
                    } else {
                        entry.content_.emplace(std::move(content));
                    }
                }

                const auto summary_node =
                    entry_node.get_child_optional("summary");
                if (summary_node) {
                    std::string summary =
                        summary_node->get_value<std::string>();
                    const auto summary_type_node =
                        summary_node->get_child_optional("<xmlattr>.type");
                    if (summary_type_node) {
                        std::string type =
                            summary_type_node->get_value<std::string>();
                        if (type == "html")
                            entry.summary_.emplace(std::move(summary),
                                                   text::type::html);
                        else if (type == "xhtml")
                            entry.summary_.emplace(std::move(summary),
                                                   text::type::xhtml);
                        else
                            entry.summary_.emplace(std::move(summary));
                    } else {
                        entry.summary_.emplace(std::move(summary));
                    }
                }

                std::vector<person> authors;
                std::vector<link> links;

                for (const auto &entry_child : entry_node)
                    if (entry_child.first == "author") {
                        const auto &author_node = entry_child.second;

                        authors.emplace_back(
                            author_node.get<std::string>("name"),
                            author_node.get_optional<std::string>("email"),
                            author_node.get_optional<std::string>("uri"));
                    } else if (entry_child.first == "link") {
                        const auto &link_attr_node =
                            entry_child.second.get_child("<xmlattr>");

                        link link;
                        link.href_ = link_attr_node.get<std::string>("href");
                        link.href_lang_ =
                            link_attr_node.get_optional<std::string>(
                                "hreflang");
                        link.length_ =
                            link_attr_node.get_optional<std::uint64_t>(
                                "length");
                        link.title_ =
                            link_attr_node.get_optional<std::string>("title");
                        link.type_ =
                            link_attr_node.get_optional<std::string>("type");

                        const auto rel_node =
                            link_attr_node.get_child_optional("rel");
                        if (rel_node) {
                            const std::string ref =
                                rel_node->get_value<std::string>();
                            if (ref == "alternate")
                                link.rel_ = rel::alternate;
                            else if (ref == "enclosure")
                                link.rel_ = rel::enclosure;
                            else if (ref == "related")
                                link.rel_ = rel::related;
                            else if (ref == "self")
                                link.rel_ = rel::self;
                            else
                                link.rel_ = rel::via;
                        }

                        links.emplace_back(std::move(link));
                    }

                if (!authors.empty())
                    entry.authors_.emplace(std::move(authors));

                if (!links.empty())
                    entry.links_.emplace(std::move(links));

                data.entries_.emplace_back(std::move(entry));
            }

        if (!authors.empty())
            data.authors_.emplace(std::move(authors));

        if (!links.empty())
            data.links_.emplace(std::move(links));

        if (!categories.empty())
            data.categories_.emplace(std::move(categories));

        if (!contributors.empty())
            data.contributors_.emplace(std::move(contributors));

        return std::move(data);
    } catch (...) {
        std::cerr << "Error!" << std::endl;
    }

    return {};
}
}
}
