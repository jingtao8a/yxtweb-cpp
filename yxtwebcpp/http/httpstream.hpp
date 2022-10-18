/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 19:34:07
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 20:19:41
 * @FilePath: /yxtweb-cpp/yxtwebcpp/http/httpstream.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _HTTPSTREAM_HPP_
#define _HTTPSTREAM_HPP_

#include "../streams/socket_stream.hpp"
#include "http_parser.hpp"
#include "http.hpp"

namespace YXTWebCpp {

namespace http {

class HttpStream : public SocketStream {
public:
    HttpStream(std::shared_ptr<Socket> sock);

    std::shared_ptr<HttpRequest> recvRequest();

    int sendResponse(std::shared_ptr<HttpResponse> rsp);
};

}

}

#endif