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
#include <feed/utilities.h>

static std::ostream &operator<<(std::ostream &stream,
                                const enum feed::atom::text::type &text_type) {
    switch (text_type) {
    case feed::atom::text::type::text:
        stream << "text";

        return stream;
    case feed::atom::text::type::html:
        stream << "html";

        return stream;
    case feed::atom::text::type::xhtml:
        stream << "xhtml";

        return stream;
    }
}

static std::ostream &operator<<(std::ostream &stream,
                                const enum feed::atom::rel &rel) {
    switch (rel) {
    case feed::atom::rel::alternate:
        stream << "alternate";

        return stream;
    case feed::atom::rel::enclosure:
        stream << "enclosure";

        return stream;
    case feed::atom::rel::related:
        stream << "related";

        return stream;
    case feed::atom::rel::self:
        stream << "self";

        return stream;
    case feed::atom::rel::via:
        stream << "via";

        return stream;
    }
}

int main(int argc, char *argv[]) {
    feed::utility::xml xml;

    const auto xml_str = xml.to_string("https://ohmyarch.github.io/atom.xml");
    if (!xml_str) {
        std::cerr << "Failed.\n";

        return 1;
    }

    const auto feed = feed::atom::parse_atom(xml_str.value());
    if (!feed) {
        std::cerr << "Failed.\n";

        return 1;
    }

    std::cout << "feed:\n  id: " << feed->id() << '\n';

    const auto &title = feed->title();
    std::cout << "  title: " << title.value() << "\n    type: " << title.type()
              << '\n';

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
                std::cout << "      rel: " << rel.value() << '\n';
            }
        }
    }

    const auto &categories = feed->categories();
    if (categories) {
        std::cout << "  categories:\n";

        for (const auto &category : categories.value()) {
            std::cout << "    category:\n      term: " << category.term()
                      << '\n';

            const auto &scheme = category.scheme();
            if (scheme)
                std::cout << "      scheme: " << scheme.value() << '\n';

            const auto &label = category.label();
            if (label)
                std::cout << "      label: " << label.value() << '\n';
        }
    }

    const auto &contributors = feed->contributors();
    if (contributors) {
        std::cout << "  contributors:\n";

        for (const auto &contributor : contributors.value()) {
            std::cout << "    contributor:\n      name: " << contributor.name()
                      << '\n';

            const auto &email = contributor.email();
            if (email)
                std::cout << "      email: " << email.value() << '\n';

            const auto &uri = contributor.uri();
            if (uri)
                std::cout << "      uri: " << uri.value() << '\n';
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
        std::cout << "  rights: " << rights->value()
                  << "\n    type: " << rights->type() << '\n';

    const auto &subtitle = feed->subtitle();
    if (subtitle)
        std::cout << "  subtitle: " << subtitle->value()
                  << "\n    type: " << subtitle->type() << '\n';

    std::cout << "  entries:\n";

    const auto &entries = feed->entries();
    for (const auto &entry : entries) {
        std::cout << "    entry:\n      id: " << entry.id() << '\n';

        const auto &title = entry.title();
        std::cout << "      title: " << title.value()
                  << "\n        type: " << title.type() << '\n';

        const auto &authors = entry.authors();
        if (authors) {
            std::cout << "      authors:\n";

            for (const auto &author : authors.value()) {
                std::cout << "        author:\n          name: "
                          << author.name() << '\n';

                const auto &email = author.email();
                if (email)
                    std::cout << "          email: " << email.value() << '\n';

                const auto &uri = author.uri();
                if (uri)
                    std::cout << "          uri: " << uri.value() << '\n';
            }
        }

        /* const auto &content = entry.content();
            if (content)
                std::cout << "      content: " << content->value()
                          << "\n        type: " << content->type() << '\n';*/

        const auto &links = entry.links();
        if (links) {
            std::cout << "      links:\n";

            for (const auto &link : links.value()) {
                std::cout << "        link:\n          href: " << link.href()
                          << '\n';

                const auto &href_lang = link.href_lang();
                if (href_lang)
                    std::cout << "          href_lang: " << href_lang.value()
                              << '\n';

                const auto &length = link.length();
                if (length)
                    std::cout << "          length: " << length.value() << '\n';

                const auto &title = link.title();
                if (title)
                    std::cout << "          title: " << title.value() << '\n';

                const auto &type = link.type();
                if (type)
                    std::cout << "          type: " << type.value() << '\n';

                const auto &rel = link.rel();
                if (rel) {
                    std::cout << "          rel: " << rel.value() << '\n';
                }
            }
        }

        /* const auto &summary = entry.summary();
         if (summary)
             std::cout << "      summary: " << summary->value()
                       << "\n        type: " << summary->type() << '\n'; */

        const auto &categories = entry.categories();
        if (categories) {
            std::cout << "      categories:\n";

            for (const auto &category : categories.value()) {
                std::cout << "        category:\n          term: "
                          << category.term() << '\n';

                const auto &scheme = category.scheme();
                if (scheme)
                    std::cout << "          scheme: " << scheme.value() << '\n';

                const auto &label = category.label();
                if (label)
                    std::cout << "          label: " << label.value() << '\n';
            }
        }

        const auto &rights = entry.rights();
        if (rights)
            std::cout << "      rights: " << rights->value()
                      << "\n        type: " << rights->type() << '\n';

        const auto &contributors = entry.contributors();
        if (contributors) {
            std::cout << "      contributors:\n";

            for (const auto &contributor : contributors.value()) {
                std::cout << "        contributor:\n          name: "
                          << contributor.name() << '\n';

                const auto &email = contributor.email();
                if (email)
                    std::cout << "          email: " << email.value() << '\n';

                const auto &uri = contributor.uri();
                if (uri)
                    std::cout << "          uri: " << uri.value() << '\n';
            }
        }
    }
}
