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

#include <feed/date_time/tz.h>
#include <feed/rss_parser.h>
#include <feed/utilities.h>

static void open_url(const std::string &uri) {
#if defined(_WIN32) && !defined(__cplusplus_winrt)
    // NOTE: Windows desktop only.
    ShellExecuteA(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    // NOTE: OS X only.
    std::string command("open \"" + uri + "\" > /dev/null 2>&1");
    std::system(command.c_str());
#else
    // NOTE: Linux/X11 only.
    std::string command("xdg-open \"" + uri + "\" > /dev/null 2>&1");
    std::system(command.c_str());
#endif
}

int main(int argc, char *argv[]) {
    feed::utility::xml xml;

    const auto xml_str = xml.to_string("https://ipn.li/kernelpanic/feed");
    if (!xml_str) {
        std::cerr << "Failed.\n";

        return 1;
    }

    const auto feed = feed::rss::parse_rss(xml_str.value());
    if (!feed) {
        std::cerr << "Failed.\n";

        return 1;
    }

    std::cout << "channel:\n"
              << "  title: " << feed->title() << '\n'
              << "  link: " << feed->link() << '\n'
              << "  description: " << feed->description() << '\n';

    const auto &copyright = feed->copyright();
    if (copyright)
        std::cout << "  copyright: " << copyright.value() << '\n';

    const auto &managing_editor = feed->managing_editor();
    if (managing_editor)
        std::cout << "  managing_editor: " << managing_editor.value() << '\n';

    const auto &web_master = feed->web_master();
    if (web_master)
        std::cout << "  web_master: " << web_master.value() << '\n';

    const auto &pub_date = feed->pub_date();
    if (pub_date)
        std::cout << "  pub_date: "
                  << date::format("%A %F %T %z %Z", pub_date.value()) << '\n';

    const auto &last_build_date = feed->last_build_date();
    if (last_build_date)
        std::cout << "  last_build_date: "
                  << date::format("%A %F %T %z %Z", last_build_date.value())
                  << '\n';

    const auto &categories = feed->categories();
    if (categories) {
        std::cout << "  categories:\n";

        for (const auto &category : categories.value()) {
            std::cout << "    category: " << category.value() << '\n';
            const auto &domain = category.domain();
            if (domain)
                std::cout << "      domain: " << domain.value() << '\n';
        }
    }

    const auto &generator = feed->generator();
    if (generator)
        std::cout << "  generator: " << generator.value() << '\n';

    const auto &docs = feed->docs();
    if (docs)
        std::cout << "  docs: " << docs.value() << '\n';

    const auto &cloud = feed->cloud();
    if (cloud)
        std::cout << "  cloud:\n"
                  << "    domain: " << cloud->domain() << '\n'
                  << "    path: " << cloud->path() << '\n'
                  << "    port: " << cloud->port() << '\n'
                  << "    protocol: "
                  << ((cloud->protocol() == feed::rss::protocol::xml_rpc)
                          ? "xml-rpc"
                          : "soap")
                  << '\n'
                  << "    register_procedure: " << cloud->register_procedure()
                  << '\n';

    const auto &ttl = feed->ttl();
    if (ttl)
        std::cout << "  ttl: " << ttl.value() << '\n';

    const auto &image = feed->image();
    if (image) {
        std::cout << "  image:\n"
                  << "    url: " << image->url() << '\n'
                  << "    title: " << image->title() << '\n'
                  << "    link: " << image->link() << '\n';

        const auto &width = image->width();
        if (width)
            std::cout << "    width: " << width.value() << '\n';

        const auto &height = image->height();
        if (height)
            std::cout << "    height: " << height.value() << '\n';

        const auto &description = image->description();
        if (description)
            std::cout << "    description: " << description.value() << '\n';
    }

    const auto &text_input = feed->text_input();
    if (text_input)
        std::cout << "  text_input:\n"
                  << "    title: " << text_input->title() << '\n'
                  << "    description: " << text_input->description() << '\n'
                  << "    name: " << text_input->name() << '\n'
                  << "    link: " << text_input->link() << '\n';

    const auto &skip_hours = feed->skip_hours();
    if (skip_hours) {
        std::cout << "  skip_hours: ";

        for (const auto hour : skip_hours.value())
            std::cout << hour << " | ";

        std::cout << "\b\b \n";
    }

    std::vector<feed::rss::day> vec{feed::rss::day::sunday,
                                    feed::rss::day::wednesday,
                                    feed::rss::day::monday};

    const auto &skip_days = feed->skip_days();
    if (skip_days) {
        std::cout << "  skip_days: ";

        for (const auto day : skip_days.value())
            switch (day) {
            case feed::rss::day::monday:
                std::cout << "Monday | ";
                break;
            case feed::rss::day::tuesday:
                std::cout << "Tuesday | ";
                break;
            case feed::rss::day::wednesday:
                std::cout << "Wednesday | ";
                break;
            case feed::rss::day::thursday:
                std::cout << "Thursday | ";
                break;
            case feed::rss::day::friday:
                std::cout << "Friday | ";
                break;
            case feed::rss::day::saturday:
                std::cout << "Saturday | ";
                break;
            case feed::rss::day::sunday:
                std::cout << "Sunday | ";
                break;
            }

        std::cout << "\b\b \n";
    }

    const auto &atom_link = feed->atom_link();
    if (atom_link) {
        std::cout << "  atom:\n"
                  << "    link:\n      href: " << atom_link->href() << '\n';

        const auto &href_lang = atom_link->href_lang();
        if (href_lang)
            std::cout << "      href_lang: " << href_lang.value() << '\n';

        const auto &length = atom_link->length();
        if (length)
            std::cout << "      length: " << length.value() << '\n';

        const auto &title = atom_link->title();
        if (title)
            std::cout << "      title: " << title.value() << '\n';

        const auto &type = atom_link->type();
        if (type)
            std::cout << "      type: " << type.value() << '\n';

        const auto &rel = atom_link->rel();
        if (rel) {
            std::cout << "      rel: ";

            switch (rel.value()) {
            case feed::atom::rel::alternate:
                std::cout << "alternate\n";
                break;
            case feed::atom::rel::enclosure:
                std::cout << "enclosure\n";
                break;
            case feed::atom::rel::related:
                std::cout << "related\n";
                break;
            case feed::atom::rel::self:
                std::cout << "self\n";
                break;
            case feed::atom::rel::via:
                std::cout << "via\n";
                break;
            }
        }
    }

    const auto &itunes = feed->itunes();
    if (itunes) {
        std::cout << "  itunes:\n";

        const auto &new_feed_url = itunes->new_feed_url();
        if (new_feed_url)
            std::cout << "    new_feed_url: " << new_feed_url.value() << '\n';
    }

    std::cout << "  items:\n";

    std::vector<std::string> urls;

    for (const auto &item : feed->items()) {
        std::cout << "    item:\n";

        const auto &title = item.title();
        if (title)
            std::cout << "      title: " << title.value() << '\n';

        const auto &link = item.link();
        if (link)
            std::cout << "      link: " << link.value() << '\n';

        // const auto &description = item.description();
        // if (description)
        // std::cout << "      description: " << description.value()
        //            << '\n';

        const auto &author = item.author();
        if (author)
            std::cout << "      author: " << author.value() << '\n';

        const auto &categories = item.categories();
        if (categories) {
            std::cout << "      categories:\n";

            for (const auto &category : categories.value()) {
                std::cout << "        category: " << category.value() << '\n';
                const auto &domain = category.domain();
                if (domain)
                    std::cout << "          domain: " << domain.value() << '\n';
            }
        }

        const auto &comments = item.comments();
        if (comments)
            std::cout << "      comments: " << comments.value() << '\n';

        const auto &enclosure = item.enclosure();
        if (enclosure) {
            std::cout << "      enclosure:\n"
                      << "        url: " << enclosure->url() << '\n';

            const auto &length = enclosure->length();
            if (length)
                std::cout << "        length: " << length.value() << '\n';

            std::cout << "        type: " << enclosure->type() << '\n';

            urls.emplace_back(enclosure->url());
        }

        const auto &guid = item.guid();
        if (guid)
            std::cout << "      guid: " << guid->value() << '\n'
                      << "        is_perma_link: " << std::boolalpha
                      << guid->is_perma_link() << '\n';

        const auto &pub_date = item.pub_date();
        if (pub_date)
            std::cout << "      pub_date: "
                      << date::format("%A %F %T %z %Z", pub_date.value())
                      << '\n';

        const auto &source = item.source();
        if (source)
            std::cout << "      source: " << source->value() << '\n'
                      << "        url: " << source->url() << '\n';
    }

    if (!urls.empty())
        open_url(urls.front());
}
