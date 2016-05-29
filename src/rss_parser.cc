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
#include <cpprest/details/basic_types.h>
#include <feed/date_time/tz.h>
#include <feed/rss_parser.h>

static std::unordered_map<std::string, std::string> offset_map = {
    {"GMT", "+0000"}, {"UTC", "+0000"}, {"UT", "+0000"},  {"EDT", "-0400"},
    {"EST", "-0500"}, {"CDT", "-0500"}, {"CST", "-0600"}, {"MDT", "-0600"},
    {"MST", "-0700"}, {"PDT", "-0700"}, {"PST", "-0800"}, {"A", "+0100"},
    {"B", "+0200"},   {"C", "+0300"},   {"D", "+0400"},   {"E", "+0500"},
    {"F", "+0600"},   {"G", "+0700"},   {"H", "+0800"},   {"I", "+0900"},
    {"K", "+1000"},   {"L", "+1100"},   {"M", "+1200"},   {"N", "-0100"},
    {"O", "-0200"},   {"P", "-0300"},   {"Q", "-0400"},   {"R", "-0500"},
    {"S", "-0600"},   {"T", "-0700"},   {"U", "-0800"},   {"V", "-0900"},
    {"W", "-1000"},   {"X", "-1100"},   {"Y", "-1200"},   {"Z", "+0000"}};

static date::second_point get_time(const std::string &str) {
    // TODO: Error Handling
    const auto pos = str.find_last_of(' ');
    const std::string utc_offset = str.substr(pos + 1);
    std::string time_str = str.substr(0, pos + 1);
    date::second_point time_point;
    if (utc_offset.size() != 5) {
        std::istringstream time_stream(time_str + offset_map[utc_offset]);
        date::parse(time_stream, "%a, %d %h %Y %T %z", time_point);
    } else {
        std::istringstream time_stream(str);
        date::parse(time_stream, "%a, %d %h %Y %T %z", time_point);
    }

    return time_point;
}

namespace feed {
boost::optional<rss_data> rss_parser::parse(const std::string &uri) {
    web::http::client::http_client client(
        utility::conversions::to_string_t(uri), http_client_config_);
    const auto response = client.request(web::http::methods::GET);

    boost::property_tree::ptree root;

    rss_data data;

    try {
        // FIXME
        utility::istringstream_t stream(
            response.get().extract_string(true).get());

        boost::property_tree::read_xml(stream, root);
        // boost::property_tree::read_xml("//home//michael//feed.xml", root);

        const auto &rss_node = root.get_child("rss");

        const auto &channel_node = rss_node.get_child("channel");

        const auto rss_attr_node = rss_node.get_child_optional("<xmlattr>");
        if (rss_attr_node) {
            const auto xmlns_atom_node =
                rss_attr_node->get_optional<std::string>("xmlns:atom");
            if (xmlns_atom_node)
                if (xmlns_atom_node.value() == "http://www.w3.org/2005/Atom") {
                    const auto atom_link_node_attr =
                        channel_node.get_child_optional("atom:link.<xmlattr>");
                    if (atom_link_node_attr) {
                        atom_link atom_link;
                        atom_link.href_ =
                            atom_link_node_attr->get<std::string>("href");
                        atom_link.href_lang_ =
                            atom_link_node_attr->get_optional<std::string>(
                                "hreflang");
                        atom_link.length_ =
                            atom_link_node_attr->get_optional<std::uint64_t>(
                                "length");
                        atom_link.title_ =
                            atom_link_node_attr->get_optional<std::string>(
                                "title");
                        atom_link.type_ =
                            atom_link_node_attr->get_optional<std::string>(
                                "type");

                        const auto rel_node =
                            atom_link_node_attr->get_child_optional("rel");
                        if (rel_node) {
                            const std::string ref =
                                rel_node->get_value<std::string>();
                            if (ref == "alternate")
                                atom_link.rel_ = rel::alternate;
                            else if (ref == "enclosure")
                                atom_link.rel_ = rel::enclosure;
                            else if (ref == "related")
                                atom_link.rel_ = rel::related;
                            else if (ref == "self")
                                atom_link.rel_ = rel::self;
                            else
                                atom_link.rel_ = rel::via;
                        }

                        data.atom_link_.emplace(std::move(atom_link));
                    }
                }

            const auto xmlns_itunes_node =
                rss_attr_node->get_optional<std::string>("xmlns:itunes");
            if (xmlns_itunes_node)
                if (xmlns_itunes_node.value() ==
                    "http://www.itunes.com/dtds/podcast-1.0.dtd") {
                    channel::itunes itunes;
                    itunes.new_feed_url_ =
                        channel_node.get_optional<std::string>(
                            "itunes:new-feed-url");

                    data.itunes_.emplace(std::move(itunes));
                }
        }

        data.title_ = channel_node.get<std::string>("title");
        data.link_ = channel_node.get<std::string>("link");
        data.description_ = channel_node.get<std::string>("description");
        data.language_ = channel_node.get_optional<std::string>("language");
        data.copyright_ = channel_node.get_optional<std::string>("copyright");
        data.managing_editor_ =
            channel_node.get_optional<std::string>("managingEditor");
        data.web_master_ = channel_node.get_optional<std::string>("webMaster");

        const auto pub_date = channel_node.get_optional<std::string>("pubDate");
        if (pub_date)
            data.pub_date_ = get_time(pub_date.value());

        const auto last_build_date =
            channel_node.get_optional<std::string>("lastBuildDate");
        if (last_build_date)
            data.last_build_date_ = get_time(last_build_date.value());

        data.generator_ = channel_node.get_optional<std::string>("generator");
        data.docs_ = channel_node.get_optional<std::string>("docs");

        const auto cloud_attr_node =
            channel_node.get_child_optional("cloud.<xmlattr>");
        if (cloud_attr_node) {
            cloud cloud;
            cloud.domain_ = cloud_attr_node->get<std::string>("domain");
            cloud.path_ = cloud_attr_node->get<std::string>("path");
            cloud.port_ = cloud_attr_node->get<std::uint16_t>("port");
            cloud.protocol_ =
                (cloud_attr_node->get<std::string>("protocol") == "xml-rpc")
                    ? protocol::xml_rpc
                    : protocol::soap;
            cloud.register_procedure_ =
                cloud_attr_node->get<std::string>("register_procedure");

            data.cloud_.emplace(std::move(cloud));
        }

        data.ttl_ = channel_node.get_optional<std::uint16_t>("ttl");

        const auto image_node = channel_node.get_child_optional("image");
        if (image_node) {
            image image;
            image.url_ = image_node->get<std::string>("url");
            image.title_ = image_node->get<std::string>("title");
            image.link_ = image_node->get<std::string>("link");
            image.width_ = image_node->get_optional<std::uint16_t>("width");
            image.height_ = image_node->get_optional<std::uint16_t>("height");
            image.description_ =
                image_node->get_optional<std::string>("description");

            data.image_.emplace(std::move(image));
        }

        const auto text_input_node =
            channel_node.get_child_optional("textInput");
        if (text_input_node) {
            text_input input;
            input.title_ = text_input_node->get<std::string>("title");
            input.description_ =
                text_input_node->get<std::string>("description");
            input.name_ = text_input_node->get<std::string>("name");
            input.link_ = text_input_node->get<std::string>("link");

            data.text_input_.emplace(std::move(input));
        }

        const auto skip_hours_node =
            channel_node.get_child_optional("skipHours");
        if (skip_hours_node) {
            std::vector<std::uint16_t> skip_hours;

            for (const auto &hour : skip_hours_node.value())
                skip_hours.emplace_back(hour.second.get_value<std::uint16_t>());

            data.skip_hours_.emplace(std::move(skip_hours));
        }

        const auto skip_days_node = channel_node.get_child_optional("skipDays");
        if (skip_days_node) {
            std::vector<day> skip_days;

            for (const auto &day_node : skip_days_node.value()) {
                const std::string day =
                    day_node.second.get_value<std::string>();
                if (day == "Monday")
                    skip_days.emplace_back(day::monday);
                else if (day == "Tuesday")
                    skip_days.emplace_back(day::tuesday);
                else if (day == "Wednesday")
                    skip_days.emplace_back(day::wednesday);
                else if (day == "Thursday")
                    skip_days.emplace_back(day::thursday);
                else if (day == "Friday")
                    skip_days.emplace_back(day::friday);
                else if (day == "Saturday")
                    skip_days.emplace_back(day::saturday);
                else
                    skip_days.emplace_back(day::sunday);
            }

            data.skip_days_.emplace(std::move(skip_days));
        }

        std::vector<category> categories;

        for (const auto &channel_child : channel_node)
            if (channel_child.first == "item") {
                item item;
                const auto &item_node = channel_child.second;
                item.title_ = item_node.get_optional<std::string>("title");
                item.link_ = item_node.get_optional<std::string>("link");
                item.description_ =
                    item_node.get_optional<std::string>("description");
                item.author_ = item_node.get_optional<std::string>("author");

                std::vector<category> categories;

                for (const auto &item_child : item_node)
                    if (item_child.first == "category") {
                        const auto &category_node = item_child.second;
                        categories.emplace_back(
                            category_node.get_value<std::string>(),
                            category_node.get_optional<std::string>(
                                "<xmlattr>.domain"));
                    }

                if (!categories.empty())
                    item.categories_.emplace(std::move(categories));

                item.comments_ =
                    item_node.get_optional<std::string>("comments");

                const auto &enclosure_attr_node =
                    item_node.get_child("enclosure.<xmlattr>");
                item.enclosure_.emplace(
                    enclosure_attr_node.get<std::string>("url"),
                    enclosure_attr_node.get_optional<std::uint64_t>("length"),
                    enclosure_attr_node.get<std::string>("type"));

                const auto guid_node = item_node.get_child_optional("guid");
                if (guid_node)
                    item.guid_.emplace(
                        guid_node->get_value<std::string>(),
                        guid_node->get_optional<bool>("<xmlattr>.isPermaLink"));

                const auto pub_date =
                    item_node.get_optional<std::string>("pubDate");
                if (pub_date)
                    item.pub_date_ = get_time(pub_date.value());

                const auto source_node = item_node.get_child_optional("source");
                if (source_node)
                    item.source_.emplace(
                        source_node->get_value<std::string>(),
                        source_node->get<std::string>("<xmlattr>.url"));

                data.items_.emplace_back(std::move(item));
            } else if (channel_child.first == "category") {
                const auto &category_node = channel_child.second;
                categories.emplace_back(category_node.get_value<std::string>(),
                                        category_node.get_optional<std::string>(
                                            "<xmlattr>.domain"));
            }

        if (!categories.empty())
            data.categories_.emplace(std::move(categories));

        return std::move(data);
    } catch (const web::http::http_exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return {};
}

bool rss_parser::set_proxy(const std::string &uri) {
    try {
        http_client_config_.set_proxy(
            web::web_proxy(utility::conversions::to_string_t(uri)));

        return true;
    } catch (const web::uri_exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return false;
}
}
