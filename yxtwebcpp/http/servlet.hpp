/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-19 11:57:22
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-19 12:51:36
 * @FilePath: /yxtweb-cpp/yxtwebcpp/http/servlet.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _SERVLET_HPP_
#define _SERVLET_HPP_
#include "http.hpp"
#include "httpstream.hpp"
#include "../mutex.hpp"
#include <unordered_map>

namespace YXTWebCpp {

namespace http {

class Servlet {
public:
    //返回结果表示是否处理成功
    virtual uint64_t handle(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response, std::shared_ptr<HttpStream> stream) = 0;
};


class FunctionServlet: public Servlet{
public:
    typedef std::function<uint64_t(std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>, std::shared_ptr<HttpStream>)> FunctionType;

    FunctionServlet(FunctionType cb);
    uint64_t handle(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response, std::shared_ptr<HttpStream> stream) override;
private:
    FunctionType m_cb;
};

class NotFoundServlet : public Servlet {
public:
    NotFoundServlet();
    uint64_t handle(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response, std::shared_ptr<HttpStream> stream) override;

private:
    std::string m_content;
};


class ServletDispatch : public Servlet {
public:

    ServletDispatch();
    uint64_t handle(std::shared_ptr<HttpRequest> request, std::shared_ptr<HttpResponse> response, std::shared_ptr<HttpStream> stream) override;


    void addServlet(const std::string& uri, std::shared_ptr<Servlet> slt);
    void addServlet(const std::string& uri, FunctionServlet::FunctionType cb);
    void addGlobServlet(const std::string& uri, std::shared_ptr<Servlet> slt);
    void addGlobServlet(const std::string& uri, FunctionServlet::FunctionType cb);
    void delServlet(const std::string& uri);
    void delGlobServlet(const std::string& uri);

    std::shared_ptr<Servlet> getServlet(const std::string& uri);
    std::shared_ptr<Servlet> getGlobServlet(const std::string& uri);

    std::shared_ptr<Servlet> getMatchedServlet(const std::string& uri);

    std::shared_ptr<Servlet> getDefault() const { return m_default;}
    void setDefault(std::shared_ptr<Servlet> v) { m_default = v;}
private:
    RWMutex m_mutex;
    /// 精准匹配servlet MAP
    std::unordered_map<std::string, std::shared_ptr<Servlet> > m_datas;
    /// 模糊匹配servlet 数组
    std::vector<std::pair<std::string, std::shared_ptr<Servlet> > > m_globs;
    /// 默认servlet，所有路径都没匹配到时使用
    std::shared_ptr<Servlet> m_default;
};

}

}


#endif