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
class image {
  public:
    image(image &&other) noexcept
        : url_(std::move(other.url_)),
          title_(std::move(other.title_)),
          link_(std::move(other.link_)),
          width_(std::move(other.width_)),
          height_(std::move(other.height_)),
          description_(std::move(other.description_)) {}

    const std::string &url() const { return url_; }
    const std::string &title() const { return title_; }
    const std::string &link() const { return link_; }
    const boost::optional<std::uint16_t> &width() const { return width_; }
    const boost::optional<std::uint16_t> &height() const { return height_; }
    const boost::optional<std::string> &description() const {
        return description_;
    }

    image &operator=(image &&other) noexcept {
        if (&other != this) {
            url_ = std::move(other.url_);
            title_ = std::move(other.title_);
            link_ = std::move(other.link_);
            width_ = std::move(other.width_);
            height_ = std::move(other.height_);
            description_ = std::move(other.description_);
        }

        return *this;
    }

  private:
    friend class parser;

    image() {}

    std::string url_;
    std::string title_;
    std::string link_;
    boost::optional<std::uint16_t> width_;
    boost::optional<std::uint16_t> height_;
    boost::optional<std::string> description_;
};

class enclosure {
  public:
    enclosure(enclosure &&other) noexcept : url_(std::move(other.url_)),
                                            length_(std::move(other.length_)),
                                            type_(std::move(other.type_)) {}

    const std::string &url() const { return url_; }
    const boost::optional<std::uint64_t> &length() const { return length_; }
    const std::string &type() const { return type_; }

    enclosure &operator=(enclosure &&other) noexcept {
        if (&other != this) {
            url_ = std::move(other.url_);
            type_ = std::move(other.type_);
        }

        return *this;
    }

  private:
    friend class parser;

    enclosure(std::string &&url, boost::optional<std::uint64_t> &&length,
              std::string &&type) noexcept : url_(std::move(url)),
                                             length_(std::move(length)),
                                             type_(std::move(type)) {}

    std::string url_;                       // Where the enclosure is located.
    boost::optional<std::uint64_t> length_; // How big it is in bytes
    std::string type_; // What its type is, a standard MIME type.
};

// All elements of an item are optional, however at least one of title or
// description must be present.
class item {
  public:
    item(item &&other) noexcept : title_(std::move(other.title_)),
                                  link_(std::move(other.link_)),
                                  description_(std::move(other.description_)),
                                  author_(std::move(other.author_)),
                                  enclosure_(std::move(other.enclosure_)) {}

    const boost::optional<std::string> &title() const { return title_; }
    const boost::optional<std::string> &link() const { return link_; }
    const boost::optional<std::string> &description() const {
        return description_;
    }
    const boost::optional<std::string> &author() const { return author_; }
    const boost::optional<class enclosure> &enclosure() const {
        return enclosure_;
    }

  private:
    friend class parser;

    item() {}

    boost::optional<std::string> title_;       // The title of the item.
    boost::optional<std::string> link_;        // The URL of the item.
    boost::optional<std::string> description_; // The item synopsis.
    boost::optional<std::string>
        author_; // Email address of the author of the item.
    boost::optional<class enclosure>
        enclosure_; // Describes a media object that
                    // is attached to the item.
};

class data {
  public:
    data(data &&other) noexcept : title_(std::move(other.title_)),
                                  link_(std::move(other.link_)),
                                  description_(std::move(other.description_)),
                                  image_(std::move(other.image_)),
                                  items_(std::move(other.items_)) {}

    const std::string &title() const { return title_; }
    const std::string &link() const { return link_; }
    const std::string &description() const { return description_; }
    const boost::optional<class image> &image() const { return image_; }

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
    boost::optional<class image> image_; // Specifies a GIF, JPEG or PNG image
                                         // that can be displayed with the
                                         // channel.

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
