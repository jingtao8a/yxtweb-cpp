/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-17 16:13:37
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 19:55:06
 * @FilePath: /yxtweb-cpp/yxtwebcpp/http/http_parser.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _HTTP_PARSER_HPP
#define _HTTP_PARSER_HPP

#include "http.hpp"
#include "http11_parser.hpp"
#include "httpclient_parser.hpp"

namespace YXTWebCpp {
namespace http {

class HttpRequestParser {
public:

    HttpRequestParser();
    //解析len长度的数据，返回值为已经解析好的数据，未解析完的数据会移动到data的头部
    size_t execute(char* data, size_t len);

    int isFinished();//如果解析为结束，则说明数据报还未接收完成

    int hasError(); 

    std::shared_ptr<HttpRequest> getData() const { return m_data;}

    void setError(int v) { m_error = v;}

    uint64_t getContentLength();

    const http_parser& getParser() const { return m_parser;}
public:
    /**
     * @brief 返回HttpRequest协议解析的缓存大小
     */
    static uint64_t GetHttpRequestBufferSize();

    /**
     * @brief 返回HttpRequest协议的最大消息体大小
     */
    static uint64_t GetHttpRequestMaxBodySize();
private:
    /// http_parser
    http_parser m_parser;
    /// HttpRequest结构
    std::shared_ptr<HttpRequest> m_data;
    /// 错误码
    /// 1000: invalid method
    /// 1001: invalid version
    /// 1002: invalid field
    int m_error;
};


class HttpResponseParser {
public:

    HttpResponseParser();

    size_t execute(char* data, size_t len, bool chunck);

    int isFinished();

    int hasError(); 

    std::shared_ptr<HttpResponse> getData() const { return m_data;}

    void setError(int v) { m_error = v;}

    uint64_t getContentLength();

    const httpclient_parser& getParser() const { return m_parser;}
public:

    static uint64_t GetHttpResponseBufferSize();

    static uint64_t GetHttpResponseMaxBodySize();
private:
    /// httpclient_parser
    httpclient_parser m_parser;
    /// HttpResponse
    std::shared_ptr<HttpResponse> m_data;
    /// 错误码
    /// 1001: invalid version
    /// 1002: invalid field
    int m_error;
};

}
} // namespace YXTWebCpp


#endif