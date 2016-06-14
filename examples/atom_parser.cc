/****************************************************************************
* *
** Copyright (C) 2016 Michael Yang
** Contact: ohmyarchlinux@gmail.com
**
** This file is part of the examples of the feed_parser.
**
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of the feed_parser library nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#include <feed/atom_parser.h>

static std::ostream &operator<<(std::ostream &stream,
                                const enum feed::text::type &text_type) {
    switch (text_type) {
    case feed::text::type::text:
        stream << "text";

        return stream;
    case feed::text::type::html:
        stream << "html";

        return stream;
    case feed::text::type::xhtml:
        stream << "xhtml";

        return stream;
    }
}

int main(int argc, char *argv[]) {
    feed::atom_parser parser;
    const auto feed = parser.parse("http://ohmyarch.github.io/atom.xml");
    if (!feed) {
        std::cerr << "Failed.\n";

        return 1;
    }

    std::cout << "feed:\n"
              << "  id: " << feed->id() << '\n';

    const auto &title = feed->title();
    std::cout << "  title: " << title.value() << '\n'
              << "    type: " << title.type() << '\n';

    const auto &authors = feed->authors();
    if (authors) {
        std::cout << "  authors:\n";

        for (const auto &author : authors.value()) {
            std::cout << "    author:\n      name: " << author.name() << '\n';

            const auto &email = author.email();
            if (email)
                std::cout << "      email: " << email.value() << '\n';

            const auto &uri = author.uri();
            if (uri)
                std::cout << "      uri: " << uri.value() << '\n';
        }
    }

    const auto &links = feed->links();
    if (links) {
        std::cout << "  links:\n";

        for (const auto &link : links.value()) {
            std::cout << "    link:\n      href: " << link.href() << '\n';

            const auto &href_lang = link.href_lang();
            if (href_lang)
                std::cout << "      href_lang: " << href_lang.value() << '\n';

            const auto &length = link.length();
            if (length)
                std::cout << "      length: " << length.value() << '\n';

            const auto &title = link.title();
            if (title)
                std::cout << "      title: " << title.value() << '\n';

            const auto &type = link.type();
            if (type)
                std::cout << "      type: " << type.value() << '\n';

            const auto &rel = link.rel();
            if (rel) {
                std::cout << "      rel: ";

                switch (rel.value()) {
                case feed::rel::alternate:
                    std::cout << "alternate\n";
                    break;
                case feed::rel::enclosure:
                    std::cout << "enclosure\n";
                    break;
                case feed::rel::related:
                    std::cout << "related\n";
                    break;
                case feed::rel::self:
                    std::cout << "self\n";
                    break;
                case feed::rel::via:
                    std::cout << "via\n";
                    break;
                }
            }
        }
    }

    const auto &generator = feed->generator();
    if (generator) {
        std::cout << "  generator: " << generator->value() << '\n';

        const auto &uri = generator->uri();
        if (uri)
            std::cout << "    uri: " << uri.value() << '\n';

        const auto &version = generator->version();
        if (version)
            std::cout << "    version: " << version.value() << '\n';
    }

    const auto &icon = feed->icon();
    if (icon)
        std::cout << "  icon: " << icon.value() << '\n';

    const auto &logo = feed->logo();
    if (logo)
        std::cout << "  logo: " << logo.value() << '\n';

    const auto &rights = feed->rights();
    if (rights)
        std::cout << "  rights: " << rights->value() << '\n'
                  << "    type: " << rights->type() << '\n';

    const auto &subtitle = feed->subtitle();
    if (subtitle)
        std::cout << "  subtitle: " << subtitle->value() << '\n'
                  << "    type: " << subtitle->type() << '\n';

    std::cout << "  entries:\n";

    const auto &entries = feed->entries();
    for (const auto &entry : entries) {
        std::cout << "    entry:\n      id: " << entry.id() << '\n';

        const auto &title = entry.title();
        std::cout << "      title: " << title.value() << '\n'
                  << "        type: " << title.type() << '\n';
    }
}
