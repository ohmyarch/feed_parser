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

#include <chrono>
#include <feed/link.h>

namespace feed {
namespace rss {
class rss_data;

class category {
  public:
    category(std::string &&value,
             boost::optional<std::string> &&domain) noexcept
        : value_(std::move(value)),
          domain_(std::move(domain)) {}
    category(const category &other)
        : value_(other.value_), domain_(other.domain_) {}
    category(category &&other) noexcept : value_(std::move(other.value_)),
                                          domain_(std::move(other.domain_)) {}

    const std::string &value() const { return value_; }
    const boost::optional<std::string> &domain() const { return domain_; }

  private:
    std::string value_;
    boost::optional<std::string>
        domain_; // A string that identifies a categorization taxonomy.
};

enum class protocol : std::uint8_t { xml_rpc, soap };

class cloud {
  public:
    cloud(const cloud &other)
        : domain_(other.domain_), path_(other.path_), port_(other.port_),
          protocol_(other.protocol_),
          register_procedure_(other.register_procedure_) {}
    cloud(cloud &&other) noexcept
        : domain_(std::move(other.domain_)),
          path_(std::move(other.path_)),
          port_(other.port_),
          protocol_(other.protocol_),
          register_procedure_(std::move(other.register_procedure_)) {}

    const std::string &domain() const { return domain_; }
    const std::string &path() const { return path_; }
    std::uint16_t port() const { return port_; }
    enum protocol protocol() const { return protocol_; }
    const std::string &register_procedure() const {
        return register_procedure_;
    }

    cloud &operator=(cloud &&other) noexcept {
        if (&other != this) {
            domain_ = std::move(other.domain_);
            path_ = std::move(other.path_);
            port_ = other.port_;
            protocol_ = other.protocol_;
            register_procedure_ = std::move(other.register_procedure_);
        }

        return *this;
    }

  private:
    friend boost::optional<rss_data> parse_rss(const std::string &xml_str);

    cloud() {}

    std::string domain_; // Domain of the cloud server.
    std::string path_;   // Location of the cloud server's responder.
    std::uint16_t port_; // Port the cloud server is running on.
    enum protocol
        protocol_; // Protocol to use for communications with the cloud
    // server (xml-rpc or soap).
    std::string register_procedure_; // Name of procedure to call to request
    // notification.
};

class image {
  public:
    image(const image &other)
        : url_(other.url_), title_(other.title_), link_(other.link_),
          width_(other.width_), height_(other.height_),
          description_(other.description_) {}
    image(image &&other) noexcept
        : url_(std::move(other.url_)),
          title_(std::move(other.title_)),
          link_(std::move(other.link_)),
          width_(other.width_),
          height_(other.height_),
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
            width_ = other.width_;
            height_ = other.height_;
            description_ = std::move(other.description_);
        }

        return *this;
    }

  private:
    friend boost::optional<rss_data> parse_rss(const std::string &xml_str);

    image() {}

    std::string url_;
    std::string title_;
    std::string link_;
    boost::optional<std::uint16_t> width_;
    boost::optional<std::uint16_t> height_;
    boost::optional<std::string> description_;
};

class text_input {
  public:
    text_input(const text_input &other)
        : title_(other.title_), description_(other.description_),
          name_(other.name_), link_(other.link_) {}
    text_input(text_input &&other) noexcept
        : title_(std::move(other.title_)),
          description_(std::move(other.description_)),
          name_(std::move(other.name_)),
          link_(std::move(other.link_)) {}

    const std::string &title() const { return title_; }
    const std::string &description() const { return description_; }
    const std::string &name() const { return name_; }
    const std::string &link() const { return link_; }

    text_input &operator=(text_input &&other) noexcept {
        if (&other != this) {
            title_ = std::move(other.title_);
            description_ = std::move(other.description_);
            name_ = std::move(other.name_);
            link_ = std::move(other.link_);
        }

        return *this;
    }

  private:
    friend boost::optional<rss_data> parse_rss(const std::string &xml_str);

    text_input() {}

    std::string
        title_; // The label of the Submit button in the text input area.
    std::string description_; // Explains the text input area.
    std::string name_; // The name of the text object in the text input area.
    std::string
        link_; // The URL of the CGI script that processes text input requests.
};

// A weekday in Greenwich Mean Time (GMT).
enum class day : std::uint8_t {
    monday,
    tuesday,
    wednesday,
    thursday,
    friday,
    saturday,
    sunday
};

namespace channel {
class itunes {
  public:
    itunes(const itunes &other) : new_feed_url_(other.new_feed_url_) {}
    itunes(itunes &&other) noexcept
        : new_feed_url_(std::move(other.new_feed_url_)) {}

    const boost::optional<std::string> &new_feed_url() const {
        return new_feed_url_;
    }

    itunes &operator=(itunes &&other) noexcept {
        if (&other != this) {
            new_feed_url_ = std::move(other.new_feed_url_);
        }

        return *this;
    }

  private:
    friend boost::optional<rss::rss_data>
    rss::parse_rss(const std::string &xml_str);

    itunes() {}

    boost::optional<std::string> new_feed_url_;
};
}

class enclosure {
  public:
    enclosure(std::string &&url, boost::optional<std::uint64_t> &&length,
              std::string &&type) noexcept : url_(std::move(url)),
                                             length_(length),
                                             type_(std::move(type)) {}
    enclosure(const enclosure &other)
        : url_(other.url_), length_(other.length_), type_(other.type_) {}
    enclosure(enclosure &&other) noexcept : url_(std::move(other.url_)),
                                            length_(other.length_),
                                            type_(std::move(other.type_)) {}

    const std::string &url() const { return url_; }
    const boost::optional<std::uint64_t> &length() const { return length_; }
    const std::string &type() const { return type_; }

  private:
    friend boost::optional<rss_data> parse_rss(const std::string &xml_str);

    std::string url_;                       // Where the enclosure is located.
    boost::optional<std::uint64_t> length_; // How big it is in bytes
    std::string type_; // What its type is, a standard MIME type.
};

class guid {
  public:
    guid(std::string &&value,
         const boost::optional<bool> &is_perma_link) noexcept
        : value_(std::move(value)),
          is_perma_link_(is_perma_link ? is_perma_link.value() : true) {}
    guid(const guid &other)
        : value_(other.value_), is_perma_link_(other.is_perma_link_) {}
    guid(guid &&other) noexcept : value_(std::move(other.value_)),
                                  is_perma_link_(other.is_perma_link_) {}

    const std::string &value() const { return value_; }
    bool is_perma_link() const { return is_perma_link_; }

  private:
    friend boost::optional<rss_data> parse_rss(const std::string &xml_str);

    std::string value_;
    bool is_perma_link_; // If its value is false, the guid may not be assumed
    // to be a url, or a url to anything in particular.
};

class source {
  public:
    source(std::string &&value, std::string &&url) noexcept
        : value_(std::move(value)),
          url_(std::move(url)) {}
    source(const source &other) : value_(other.value_), url_(other.url_) {}
    source(source &&other) noexcept : value_(std::move(other.value_)),
                                      url_(std::move(other.url_)) {}

    const std::string &value() const { return value_; }
    const std::string &url() const { return url_; }

  private:
    friend boost::optional<rss_data> parse_rss(const std::string &xml_str);

    std::string value_;
    std::string url_;
};

// All elements of an item are optional, however at least one of title or
// description must be present.
class item {
  public:
    item(const item &other)
        : title_(other.title_), link_(other.link_),
          description_(other.description_), author_(other.author_),
          categories_(other.categories_), comments_(other.comments_),
          enclosure_(other.enclosure_), guid_(other.guid_),
          pub_date_(other.pub_date_), source_(other.source_) {}
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
    friend boost::optional<rss_data> parse_rss(const std::string &xml_str);

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

class rss_data {
  public:
    rss_data() {}
    rss_data(const rss_data &other)
        : title_(other.title_), link_(other.link_),
          description_(other.description_), language_(other.language_),
          copyright_(other.copyright_),
          managing_editor_(other.managing_editor_),
          web_master_(other.web_master_), pub_date_(other.pub_date_),
          last_build_date_(other.last_build_date_),
          categories_(other.categories_), generator_(other.generator_),
          docs_(other.docs_), cloud_(other.cloud_), ttl_(other.ttl_),
          image_(other.image_), text_input_(other.text_input_),
          skip_hours_(other.skip_hours_), skip_days_(other.skip_days_),
          items_(other.items_), atom_link_(other.atom_link_),
          itunes_(other.itunes_) {}
    rss_data(rss_data &&other) noexcept
        : title_(std::move(other.title_)),
          link_(std::move(other.link_)),
          description_(std::move(other.description_)),
          language_(std::move(other.language_)),
          copyright_(std::move(other.copyright_)),
          managing_editor_(std::move(other.managing_editor_)),
          web_master_(std::move(other.web_master_)),
          pub_date_(other.pub_date_),
          last_build_date_(other.last_build_date_),
          categories_(std::move(other.categories_)),
          generator_(std::move(other.generator_)),
          docs_(std::move(other.docs_)),
          cloud_(std::move(other.cloud_)),
          ttl_(other.ttl_),
          image_(std::move(other.image_)),
          text_input_(std::move(other.text_input_)),
          skip_hours_(std::move(other.skip_hours_)),
          skip_days_(std::move(other.skip_days_)),
          items_(std::move(other.items_)),
          atom_link_(std::move(other.atom_link_)),
          itunes_(std::move(other.itunes_)) {}

    const std::string &title() const { return title_; }
    const std::string &link() const { return link_; }
    const std::string &description() const { return description_; }
    const boost::optional<std::string> &language() const { return language_; }
    const boost::optional<std::string> &copyright() const { return copyright_; }
    const boost::optional<std::string> &managing_editor() const {
        return managing_editor_;
    }
    const boost::optional<std::string> &web_master() const {
        return web_master_;
    }
    const boost::optional<std::chrono::time_point<std::chrono::system_clock,
                                                  std::chrono::seconds>> &
    pub_date() const {
        return pub_date_;
    }
    const boost::optional<std::chrono::time_point<std::chrono::system_clock,
                                                  std::chrono::seconds>> &
    last_build_date() const {
        return last_build_date_;
    }
    const boost::optional<std::vector<class category>> &categories() const {
        return categories_;
    }
    const boost::optional<std::string> &generator() const { return generator_; }
    const boost::optional<std::string> &docs() const { return docs_; }
    const boost::optional<class cloud> &cloud() const { return cloud_; }
    const boost::optional<std::uint16_t> &ttl() const { return ttl_; }
    const boost::optional<class image> &image() const { return image_; }
    const boost::optional<class text_input> &text_input() const {
        return text_input_;
    }
    const boost::optional<std::vector<std::uint16_t>> &skip_hours() const {
        return skip_hours_;
    }
    const boost::optional<std::vector<day>> &skip_days() const {
        return skip_days_;
    }
    const std::vector<item> &items() const { return items_; }
    const boost::optional<class atom::link> &atom_link() const {
        return atom_link_;
    }
    const boost::optional<class channel::itunes> &itunes() const {
        return itunes_;
    }

    rss_data &operator=(rss_data &&other) noexcept {
        if (&other != this) {
            title_ = std::move(other.title_);
            link_ = std::move(other.link_);
            description_ = std::move(other.description_);
            language_ = std::move(other.language_);
            copyright_ = std::move(other.copyright_);
            managing_editor_ = std::move(other.managing_editor_);
            web_master_ = std::move(other.web_master_);
            pub_date_ = other.pub_date_;
            last_build_date_ = other.last_build_date_;
            categories_ = std::move(other.categories_);
            generator_ = std::move(other.generator_);
            docs_ = std::move(other.docs_);
            cloud_ = std::move(other.cloud_);
            ttl_ = other.ttl_;
            image_ = std::move(other.image_);
            text_input_ = std::move(other.text_input_);
            skip_hours_ = std::move(other.skip_hours_);
            skip_days_ = std::move(other.skip_days_);
            items_ = std::move(other.items_);
            atom_link_ = std::move(other.atom_link_);
            itunes_ = std::move(other.itunes_);
        }

        return *this;
    }

  private:
    friend boost::optional<rss_data> parse_rss(const std::string &xml_str);

    std::string title_; // The name of the channel.
    std::string
        link_; // The URL to the HTML website corresponding to the channel.
    std::string description_; // Phrase or sentence describing the channel.
    boost::optional<std::string>
        language_; // The language the channel is written in.
    boost::optional<std::string>
        copyright_; // Copyright notice for content in the channel.
    boost::optional<std::string> managing_editor_; // Email address for person
    // responsible for editorial
    // content.
    boost::optional<std::string> web_master_; // Email address for person
    // responsible for technical
    // issues relating to channel.
    boost::optional<std::chrono::time_point<std::chrono::system_clock,
                                            std::chrono::seconds>>
        pub_date_;
    boost::optional<std::chrono::time_point<std::chrono::system_clock,
                                            std::chrono::seconds>>
        last_build_date_;
    boost::optional<std::vector<class category>> categories_;
    boost::optional<std::string> generator_; // A string indicating the program
    // used to generate the channel.
    boost::optional<std::string> docs_; // A URL that points to the
    // documentation for the format used in
    // the RSS file.
    boost::optional<class cloud>
        cloud_; // Indicates that updates to the feed can be
    // monitored using a web service that
    // implements the RssCloud application
    // programming interface.
    boost::optional<std::uint16_t> ttl_; // a number of minutes that indicates
    // how long a channel can be cached
    // before refreshing from the source
    boost::optional<class image> image_; // Specifies a GIF, JPEG or PNG image
    // that can be displayed with the
    // channel.
    boost::optional<class text_input> text_input_; // Specifies a text input box
    // that can be displayed with
    // the channel.
    boost::optional<std::vector<std::uint16_t>> skip_hours_; // A hint for
    // aggregators
    // telling them
    // which days they
    // can skip.
    boost::optional<std::vector<day>> skip_days_; // A hint for aggregators
    // telling them which days
    // they can skip.
    std::vector<item> items_;
    boost::optional<class atom::link>
        atom_link_; // A relationship between a web
    // resource (such as a page) and an
    // RSS channel or item.
    boost::optional<class channel::itunes> itunes_;
};

boost::optional<rss_data> parse_rss(const std::string &xml_str);
}
}
