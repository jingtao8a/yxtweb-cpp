/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-16 18:21:59
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-16 21:03:57
 * @FilePath: /yxtweb-cpp/yxtwebcpp/http/http.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "http.hpp"
#include "log.hpp"
#include <string.h>
#include "util.hpp"

namespace YXTWebCpp {

namespace http {

HttpMethod StringToHttpMethod(const std::string& m) {
#define XX(num, name, string) \
    if(strcmp(#string, m.c_str()) == 0) { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::INVALID_METHOD;
}

HttpMethod CharsToHttpMethod(const char* m) {
#define XX(num, name, string) \
    if(strncmp(#string, m, strlen(#string)) == 0) { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::INVALID_METHOD;
}

static const char* s_method_string[] = {
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};

const char* HttpMethodToString(const HttpMethod& m) {
    uint32_t idx = (uint32_t)m;
    if(idx >= (sizeof(s_method_string) / sizeof(s_method_string[0]))) {
        return "<unknown>";
    }
    return s_method_string[idx];
}

const char* HttpStatusToString(const HttpStatus& s) {
    switch(s) {
#define XX(code, name, msg) \
        case HttpStatus::name: \
            return #msg;
        HTTP_STATUS_MAP(XX);
#undef XX
        default:
            return "<unknown>";
    }
}

bool CaseInsensitiveLess::operator()(const std::string& lhs, const std::string& rhs) const {
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

std::shared_ptr<HttpResponse> HttpRequest::createResponse() {
    std::shared_ptr<HttpResponse> rsp(new HttpResponse(getVersion()
                            ,isClose()));
    return rsp;
}

HttpRequest::HttpRequest(uint8_t version, bool close)
    :m_method(HttpMethod::GET)
    ,m_version(version)
    ,m_close(close)
    ,m_websocket(false)
    ,m_parserParamFlag(0)
    ,m_path("/") {
}

std::string HttpRequest::getHeader(const std::string& key
                            ,const std::string& def) const {
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

std::string HttpRequest::getParam(const std::string& key
                            ,const std::string& def) {
    initQueryParam();
    initBodyParam();
    auto it = m_params.find(key);
    return it == m_params.end() ? def : it->second;
}

std::string HttpRequest::getCookie(const std::string& key
                            ,const std::string& def) {
    initCookies();
    auto it = m_cookies.find(key);
    return it == m_cookies.end() ? def : it->second;
}

void HttpRequest::setHeader(const std::string& key, const std::string& val) {
    m_headers[key] = val;
}

void HttpRequest::setParam(const std::string& key, const std::string& val) {
    m_params[key] = val;
}

void HttpRequest::setCookie(const std::string& key, const std::string& val) {
    m_cookies[key] = val;
}

void HttpRequest::delHeader(const std::string& key) {
    m_headers.erase(key);
}

void HttpRequest::delParam(const std::string& key) {
    m_params.erase(key);
}

void HttpRequest::delCookie(const std::string& key) {
    m_cookies.erase(key);
}

bool HttpRequest::hasHeader(const std::string& key, std::string* val) {
    auto it = m_headers.find(key);
    if(it == m_headers.end()) {
        return false;
    }
    if(val) {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasParam(const std::string& key, std::string* val) {
    initQueryParam();
    initBodyParam();
    auto it = m_params.find(key);
    if(it == m_params.end()) {
        return false;
    }
    if(val) {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasCookie(const std::string& key, std::string* val) {
    initCookies();
    auto it = m_cookies.find(key);
    if(it == m_cookies.end()) {
        return false;
    }
    if(val) {
        *val = it->second;
    }
    return true;
}

std::string HttpRequest::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

std::ostream& HttpRequest::dump(std::ostream& os) const {
    os << HttpMethodToString(m_method) << " "
        << m_path
        << (m_query.empty() ? "" : "?")
        << m_query
        << (m_fragment.empty() ? "" : "#")
        << m_fragment
        << " HTTP/"
        << ((uint32_t)(m_version >> 4))
        << "."
        << ((uint32_t)(m_version & 0x0F))
        << "\r\n";
    if(!m_websocket) {
        os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";
    }
    for(auto& i : m_headers) {
        if(!m_websocket && strcasecmp(i.first.c_str(), "connection") == 0) {
            continue;
        }
        os << i.first << ": " << i.second << "\r\n";
    }

    if(!m_body.empty()) {
        os << "content-length: " << m_body.size() << "\r\n\r\n"
            << m_body;
    } else {
        os << "\r\n";
    }
    return os;
}

void HttpRequest::init() {
    std::string conn = getHeader("connection");
    if(!conn.empty()) {
        if(strcasecmp(conn.c_str(), "keep-alive") == 0) {
            m_close = false;
        } else {
            m_close = true;
        }
    }
}

void HttpRequest::initParam() {
    initQueryParam();
    initBodyParam();
    initCookies();
}

void HttpRequest::initQueryParam() {
    if(m_parserParamFlag & 0x1) {
        return;
    }

#define PARSE_PARAM(str, m, flag, trim) \
    size_t pos = 0; \
    do { \
        size_t last = pos; \
        pos = str.find('=', pos); \
        if(pos == std::string::npos) { \
            break; \
        } \
        size_t key = pos; \
        pos = str.find(flag, pos); \
        m.insert(std::make_pair(trim(str.substr(last, key - last)), \
                    YXTWebCpp::StringUtil::UrlDecode(str.substr(key + 1, pos - key - 1)))); \
        if(pos == std::string::npos) { \
            break; \
        } \
        ++pos; \
    } while(true);

    PARSE_PARAM(m_query, m_params, '&',);
    m_parserParamFlag |= 0x1;
}

void HttpRequest::initBodyParam() {
    if(m_parserParamFlag & 0x2) {
        return;
    }
    std::string content_type = getHeader("content-type");
    if(strcasestr(content_type.c_str(), "application/x-www-form-urlencoded") == nullptr) {
        m_parserParamFlag |= 0x2;
        return;
    }
    PARSE_PARAM(m_body, m_params, '&',);
    m_parserParamFlag |= 0x2;
}

void HttpRequest::initCookies() {
    if(m_parserParamFlag & 0x4) {
        return;
    }
    std::string cookie = getHeader("cookie");
    if(cookie.empty()) {
        m_parserParamFlag |= 0x4;
        return;
    }
    PARSE_PARAM(cookie, m_cookies, ';', YXTWebCpp::StringUtil::Trim);
    m_parserParamFlag |= 0x4;
}

}

}