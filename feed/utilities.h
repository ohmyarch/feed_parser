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

using namespace utility;

namespace feed {
namespace utility {
class xml {
  public:
    boost::optional<std::string> to_string(const std::string uri) const {
        try {
            web::http::client::http_client client(conversions::to_string_t(uri),
                                                  http_client_config_);
            const auto response = client.request(web::http::methods::GET);

            return conversions::to_utf8string(
                response.get().extract_string(true).get()); // FIXME
        } catch (const web::uri_exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        } catch (const web::http::http_exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        return {};
    }
    bool set_proxy(const std::string &uri) {
        try {
            http_client_config_.set_proxy(
                web::web_proxy(conversions::to_string_t(uri)));

            return true;
        } catch (const web::uri_exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        return false;
    }
    void clear_proxy() { http_client_config_.set_proxy(web::web_proxy()); }

  protected:
    web::http::client::http_client_config http_client_config_;
};
}
}
