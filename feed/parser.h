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

class category {
  public:
    category(std::string &&value,
             boost::optional<std::string> &&domain) noexcept
        : value_(std::move(value)),
          domain_(std::move(domain)) {}
    category(category &&other) noexcept : value_(std::move(other.value_)),
                                          domain_(std::move(other.value_)) {}

    const std::string &value() const { return value_; }
    const boost::optional<std::string> &domain() const { return domain_; }

  private:
    std::string value_;
    boost::optional<std::string>
        domain_; // A string that identifies a categorization taxonomy.
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

class guid {
  public:
    guid(guid &&other) noexcept : value_(std::move(other.value_)),
                                  is_perma_link_(other.is_perma_link_) {}

    const std::string &value() const { return value_; }
    bool is_perma_link() const { return is_perma_link_; }

    guid &operator=(guid &&other) noexcept {
        if (&other != this) {
            value_ = std::move(other.value_);
            is_perma_link_ = other.is_perma_link_;
        }

        return *this;
    }

  private:
    friend class parser;

    guid(std::string &&value,
         const boost::optional<bool> &is_perma_link) noexcept
        : value_(std::move(value)),
          is_perma_link_(is_perma_link ? is_perma_link.value() : true) {}

    std::string value_;
    bool is_perma_link_; // If its value is false, the guid may not be assumed
                         // to be a url, or a url to anything in particular.
};

class source {
  public:
    source(source &&other) noexcept : value_(std::move(other.value_)),
                                      url_(std::move(other.url_)) {}

    const std::string &value() const { return value_; }
    const std::string &url() const { return url_; }

    source &operator=(source &&other) noexcept {
        if (&other != this) {
            value_ = std::move(other.value_);
            url_ = std::move(other.url_);
        }

        return *this;
    }

  private:
    friend class parser;

    source(std::string &&value, std::string &&url) noexcept
        : value_(std::move(value)),
          url_(std::move(url)) {}

    std::string value_;
    std::string url_;
};

// All elements of an item are optional, however at least one of title or
// description must be present.
class item {
  public:
    item(item &&other) noexcept : title_(std::move(other.title_)),
                                  link_(std::move(other.link_)),
                                  description_(std::move(other.description_)),
                                  author_(std::move(other.author_)),
                                  categories_(std::move(other.categories_)),
                                  comments_(std::move(other.comments_)),
                                  enclosure_(std::move(other.enclosure_)),
                                  guid_(std::move(other.guid_)),
                                  pub_date_(other.pub_date_),
                                  source_(std::move(other.source_)) {}

    const boost::optional<std::string> &title() const { return title_; }
    const boost::optional<std::string> &link() const { return link_; }
    const boost::optional<std::string> &description() const {
        return description_;
    }
    const boost::optional<std::string> &author() const { return author_; }
    const boost::optional<std::vector<class category>> &categories() const {
        return categories_;
    }
    const boost::optional<std::string> &comments() const { return comments_; }
    const boost::optional<class enclosure> &enclosure() const {
        return enclosure_;
    }
    const boost::optional<class guid> &guid() const { return guid_; }
    const boost::optional<std::chrono::time_point<std::chrono::system_clock,
                                                  std::chrono::seconds>> &
    pub_date() const {
        return pub_date_;
    }
    const boost::optional<class source> &source() const { return source_; }

  private:
    friend class parser;

    item() {}

    boost::optional<std::string> title_;       // The title of the item.
    boost::optional<std::string> link_;        // The URL of the item.
    boost::optional<std::string> description_; // The item synopsis.
    boost::optional<std::string>
        author_; // Email address of the author of the item.
    boost::optional<std::vector<class category>>
        categories_; // Includes the item in one or more categories.
    boost::optional<std::string>
        comments_; // URL of a page for comments relating to the item.
    boost::optional<class enclosure>
        enclosure_; // Describes a media object that
                    // is attached to the item.
    boost::optional<class guid>
        guid_; // A string that uniquely identifies the item.
    boost::optional<std::chrono::time_point<std::chrono::system_clock,
                                            std::chrono::seconds>>
        pub_date_;
    boost::optional<class source>
        source_; // The RSS channel that the item came from.
};

class data {
  public:
    data(data &&other) noexcept
        : title_(std::move(other.title_)),
          link_(std::move(other.link_)),
          description_(std::move(other.description_)),
          copyright_(std::move(other.copyright_)),
          managing_editor_(std::move(other.managing_editor_)),
          web_master_(std::move(other.web_master_)),
          image_(std::move(other.image_)),
          items_(std::move(other.items_)) {}

    const std::string &title() const { return title_; }
    const std::string &link() const { return link_; }
    const std::string &description() const { return description_; }
    const boost::optional<std::string> &copyright() const { return copyright_; }
    const boost::optional<std::string> &managing_editor() const {
        return managing_editor_;
    }
    const boost::optional<std::string> &web_master() const {
        return web_master_;
    }
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
