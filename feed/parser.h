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

#pragma once

#include <boost/optional.hpp>
#include <cpprest/http_client.h>
#include <string>

namespace feed {
class image {};

// All elements of an item are optional, however at least one of title or
// description must be present.
class item {
  public:
    item(item &&other) noexcept : title_(std::move(other.title_)),
                                  link_(std::move(other.link_)),
                                  description_(std::move(other.description_)),
                                  author_(std::move(other.author_)) {}

    const boost::optional<std::string> &title() const { return title_; }
    const boost::optional<std::string> &link() const { return link_; }
    const boost::optional<std::string> &description() const {
        return description_;
    }
    const boost::optional<std::string> &author() const { return author_; }
    // const boost::optional<std::string> &enclosure() const { return
    // enclosure_; }

  private:
    friend class parser;

    item() {}

    boost::optional<std::string> title_;       // The title of the item.
    boost::optional<std::string> link_;        // The URL of the item.
    boost::optional<std::string> description_; // The item synopsis.
    boost::optional<std::string>
        author_; // Email address of the author of the item.
    // boost::optional<std::string>
    //     enclosure_; // Describes a media object that is attached to the item.
};

class data {
  public:
    data(data &&other) noexcept : title_(std::move(other.title_)),
                                  link_(std::move(other.link_)),
                                  description_(std::move(other.description_)),
                                  items_(std::move(other.items_)) {}

    const std::string &title() const { return title_; }
    const std::string &link() const { return link_; }
    const std::string &description() const { return description_; }

    const std::vector<item> &items() const { return items_; }

  private:
    friend class parser;

    data() {}

    std::string title_; // The name of the channel.
    std::string
        link_; // The URL to the HTML website corresponding to the channel.
    std::string description_; // Phrase or sentence describing the channel.
    boost::optional<std::string>
        copyright_; // Copyright notice for content in the channel.
    boost::optional<std::string> managing_editor_; // Email address for person
                                                   // responsible for editorial
                                                   // content.
    boost::optional<std::string> web_master_;      // Email address for person
                                                   // responsible for technical
    // issues relating to channel.
    boost::optional<std::string> generator_; // A string indicating the program
                                             // used to generate the channel.
    boost::optional<std::string> docs_;      // A URL that points to the
    // documentation for the format used in
    // the RSS file.

    std::vector<item> items_;
};

class parser { // rss 2.0
  public:
    boost::optional<data> parse(const std::string &uri);

    bool set_proxy(const std::string &uri);

  protected:
    web::http::client::http_client_config http_client_config_;
};
}
