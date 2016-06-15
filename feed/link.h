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

namespace feed {
namespace rss {
class rss_parser;
}

namespace atom {
enum class rel : std::uint8_t {
    alternate, // An alternate representation, such as a web page containing the
               // same content as a feed entry.
    enclosure, // A media object such as an audio or video file.
    related,   // A related resource.
    self,      // The feed itself.
    via // The original source that authored the entry, when it's not the feed
        // publisher.
};

class link {
  public:
    link(link &&other) noexcept : href_(std::move(other.href_)),
                                  href_lang_(std::move(other.href_lang_)),
                                  length_(other.length_),
                                  title_(std::move(other.title_)),
                                  type_(std::move(other.type_)),
                                  rel_(other.rel_) {}

    const std::string &href() const { return href_; }
    const boost::optional<std::string> &href_lang() const { return href_lang_; }
    const boost::optional<std::uint64_t> &length() const { return length_; }
    const boost::optional<std::string> &title() const { return title_; }
    const boost::optional<std::string> &type() const { return type_; }
    const boost::optional<enum rel> &rel() const { return rel_; }

  private:
    friend class rss::rss_parser;
    friend class atom_parser;

    link() {}

    std::string href_;                       // The URL of the related resource.
    boost::optional<std::string> href_lang_; // The language used by the related
                                             // resource using an HTML language
                                             // code.
    boost::optional<std::uint64_t> length_;  // The resource's size, in bytes.
    boost::optional<std::string>
        title_; // A human-readable description of the resource.
    boost::optional<std::string> type_; // The resource's MIME media type.
    boost::optional<enum rel>
        rel_; // Contains a keyword that identifies the nature
              // of the relationship between the linked resouce
              // and the element.
};
}
}
