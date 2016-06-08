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

#include <boost/optional.hpp>
#include <cpprest/http_client.h>

namespace feed {
class text {
  public:
    enum class type : std::uint8_t { text, html, xhtml };

    text(std::string &&value, type type) noexcept : value_(std::move(value)),
                                                    type_(type) {}
    text(text &&other) noexcept : value_(std::move(other.value_)),
                                  type_(other.type_) {}

    const std::string &value() const { return value_; }
    type type() const { return type_; }

  private:
    friend class atom_data;
    friend class atom_parser;

    text() : type_(type::text) {}

    std::string value_;
    enum type type_;
};

class generator {
  public:
    generator(std::string &&value, boost::optional<std::string> &&uri,
              boost::optional<std::string> &&version) noexcept
        : value_(std::move(value)),
          uri_(std::move(uri)),
          version_(std::move(version)) {}
    generator(generator &&other) noexcept
        : value_(std::move(other.value_)),
          uri_(std::move(other.uri_)),
          version_(std::move(other.version_)) {}

    const std::string &value() const { return value_; }
    const boost::optional<std::string> &uri() const { return uri_; }
    const boost::optional<std::string> &version() const { return version_; }

  private:
    std::string value_;
    boost::optional<std::string> uri_;
    boost::optional<std::string> version_;
};

class entry {
  public:
    entry(entry &&other) noexcept : id_(std::move(other.id_)),
                                    title_(other.title_) {}

    const std::string &id() const { return id_; }
    const text &title() const { return title_; }

  private:
    std::string id_; // Identifies the entry using a universally unique and
                     // permanent URI.
    text title_;     // Contains a human readable title for the entry.
};

class atom_data {
  public:
    atom_data(atom_data &&other) noexcept
        : id_(std::move(other.id_)),
          title_(std::move(other.title_)),
          generator_(std::move(other.generator_)),
          icon_(std::move(other.icon_)),
          logo_(std::move(other.logo_)),
          rights_(std::move(other.rights_)),
          subtitle_(std::move(other.subtitle_)),
          entries_(std::move(other.entries_)) {}

    const std::string &id() const { return id_; }
    const text &title() const { return title_; }
    const boost::optional<class generator> &generator() const {
        return generator_;
    }
    const boost::optional<std::string> &icon() const { return icon_; }
    const boost::optional<std::string> &logo() const { return logo_; }
    const boost::optional<text> &rights() const { return rights_; }
    const boost::optional<text> &subtitle() const { return subtitle_; }
    const std::vector<entry> &entries() const { return entries_; }

  private:
    friend class atom_parser;

    atom_data() {}

    std::string id_; // Identifies the feed using a universally unique and
                     // permanent URI.
    text title_;     // Contains a human readable title for the feed.
    boost::optional<class generator>
        generator_; // Identifies the software used to generate the feed.
    boost::optional<std::string> icon_; // Identifies a small image which
                                        // provides iconic visual identification
                                        // for the feed. Icons should be square.
    boost::optional<std::string> logo_; // Identifies a larger image which
    // provides visual identification for
    // the feed. Images should be twice as
    // wide as they are tall.
    boost::optional<text> rights_;   // Conveys information about rights, e.g.
                                     // copyrights, held in and over the feed.
    boost::optional<text> subtitle_; // Contains a human-readable description or
    // subtitle for the feed.
    std::vector<entry> entries_;
};

class atom_parser {
  public:
    boost::optional<atom_data> parse(const std::string &uri);

  private:
    web::http::client::http_client_config http_client_config_;
};
}