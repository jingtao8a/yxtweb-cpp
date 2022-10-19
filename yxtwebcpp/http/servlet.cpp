/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-19 11:57:45
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-19 13:01:03
 * @FilePath: /yxtweb-cpp/yxtwebcpp/http/servlet.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "servlet.hpp"
#include <fnmatch.h>

namespace YXTWebCpp {

namespace http {

FunctionServlet::FunctionServlet(FunctionType cb):m_cb(cb) {}

uint64_t FunctionServlet::handle(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response, std::shared_ptr<HttpStream> stream) {
    return m_cb(request, response, stream);
}


NotFoundServlet::NotFoundServlet():m_content("<html><head><title>404 Not Found"
        "</title></head><body><center><h1>404 Not Found</h1></center>"
        "<hr><center></center></body></html>") {}

uint64_t NotFoundServlet::handle(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response, std::shared_ptr<HttpStream> stream) {
    response->setStatus(HttpStatus::NOT_FOUND);
    response->setHeader("Content-Type", "text/html");
    response->setBody(m_content);
    return 0;
}

ServletDispatch::ServletDispatch() {
    m_default.reset(new NotFoundServlet);
}

uint64_t ServletDispatch::handle(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response, std::shared_ptr<HttpStream> stream) {
    auto slt = getMatchedServlet(request->getPath());
    slt->handle(request, response, stream);
    return 0;
}

void ServletDispatch::addServlet(const std::string& uri, std::shared_ptr<Servlet> slt) {
    WriteScopedLockImpl<RWMutex> guard(m_mutex);
    m_datas[uri] = slt;
}

void ServletDispatch::addServlet(const std::string& uri, FunctionServlet::FunctionType cb) {
    WriteScopedLockImpl<RWMutex> guard(m_mutex);
    m_datas[uri] = std::make_shared<FunctionServlet>(cb);
}

void ServletDispatch::addGlobServlet(const std::string& uri, std::shared_ptr<Servlet> slt) {
    delGlobServlet(uri);
    WriteScopedLockImpl<RWMutex> guard(m_mutex);
    m_globs.push_back(std::make_pair(uri, slt));
}

void ServletDispatch::addGlobServlet(const std::string& uri, FunctionServlet::FunctionType cb) {
    WriteScopedLockImpl<RWMutex> guard(m_mutex);
    addGlobServlet(uri, std::make_shared<FunctionServlet>(cb));
}

void ServletDispatch::delServlet(const std::string& uri) {
    WriteScopedLockImpl<RWMutex> guard(m_mutex);
    m_datas.erase(uri);
}

void ServletDispatch::delGlobServlet(const std::string& uri) {
    WriteScopedLockImpl<RWMutex> guard(m_mutex);
    for (auto iter = m_globs.begin(); iter != m_globs.end(); ++iter) {
        if (iter->first == uri) {
            m_globs.erase(iter);
            break;
        }
    }
}

std::shared_ptr<Servlet> ServletDispatch::getServlet(const std::string& uri) {
    ReadScopedLockImpl<RWMutex> guard(m_mutex);
    auto iter = m_datas.find(uri);
    if (iter == m_datas.end()) {
        return nullptr;
    }
    return iter->second;
}

std::shared_ptr<Servlet> ServletDispatch::getGlobServlet(const std::string& uri) {
    ReadScopedLockImpl<RWMutex> guard(m_mutex);
    for (auto iter = m_globs.begin(); iter != m_globs.end(); ++iter) {
        if(!fnmatch(iter->first.c_str(), uri.c_str(), 0)) {
            return iter->second;
        }
    }
    return nullptr;
}

std::shared_ptr<Servlet> ServletDispatch::getMatchedServlet(const std::string& uri) {
    auto slt = getServlet(uri);
    if (slt != nullptr) {
        return slt;
    }
    slt = getGlobServlet(uri);
    if (slt != nullptr) {
        return slt;
    }
    ReadScopedLockImpl<RWMutex> guard(m_mutex);
    return m_default;
}

}

}