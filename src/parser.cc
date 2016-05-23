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
#include <feed/parser.h>

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
boost::optional<data> parser::parse(const std::string &uri) {
    web::http::client::http_client client(
        utility::conversions::to_string_t(uri), http_client_config_);
    const auto response = client.request(web::http::methods::GET);

    boost::property_tree::ptree root;

    data data;

    try {
        // FIXME
        utility::istringstream_t stream(
            response.get().extract_string(true).get());

        boost::property_tree::read_xml(stream, root);
        // boost::property_tree::read_xml("//home//michael//feed.xml", root);

        const auto &rss_node = root.get_child("rss");

        const auto &channel_node = rss_node.get_child("channel");
        data.title_ = channel_node.get<std::string>("title");
        data.link_ = channel_node.get<std::string>("link");
        data.description_ = channel_node.get<std::string>("description");
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

            data.image_ = std::move(image);
        }

        for (const auto &channel_child : channel_node)
            if (channel_child.first == "item") {
                item item;
                const auto &item_node = channel_child.second;
                item.title_ = item_node.get_optional<std::string>("title");
                item.link_ = item_node.get_optional<std::string>("link");
                item.description_ =
                    item_node.get_optional<std::string>("description");
                item.author_ = item_node.get_optional<std::string>("author");

                std::vector<category> categories_;

                for (const auto &item_child : item_node)
                    if (item_child.first == "category") {
                        const auto &category_node = item_child.second;
                        categories_.emplace_back(
                            category_node.get_value<std::string>(),
                            category_node.get_optional<std::string>(
                                "<xmlattr>.domain"));
                    }

                if (!categories_.empty())
                    item.categories_ = std::move(categories_);

                item.comments_ =
                    item_node.get_optional<std::string>("comments");

                const auto &enclosure_node =
                    item_node.get_child("enclosure.<xmlattr>");
                item.enclosure_ = enclosure(
                    enclosure_node.get<std::string>("url"),
                    enclosure_node.get_optional<std::uint64_t>("length"),
                    enclosure_node.get<std::string>("type"));

                const auto guid_node = item_node.get_child_optional("guid");
                if (guid_node)
                    item.guid_ = guid(
                        guid_node->get_value<std::string>(),
                        guid_node->get_optional<bool>("<xmlattr>.isPermaLink"));

                const auto pub_date =
                    item_node.get_optional<std::string>("pubDate");
                if (pub_date)
                    item.pub_date_ = get_time(pub_date.value());

                const auto source_node = item_node.get_child_optional("source");
                if (source_node)
                    item.source_ =
                        source(source_node->get_value<std::string>(),
                               source_node->get<std::string>("<xmlattr>.url"));

                data.items_.emplace_back(std::move(item));
            } else if (channel_child.first == "category") {
                ;
            }

        return std::move(data);
    } catch (const web::http::http_exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return {};
}

bool parser::set_proxy(const std::string &uri) {
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
