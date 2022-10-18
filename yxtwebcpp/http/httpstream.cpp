/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-18 19:34:15
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-18 20:02:12
 * @FilePath: /yxtweb-cpp/yxtwebcpp/http/httpstream.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "httpstream.hpp"
#include <sstream>

namespace YXTWebCpp {

namespace http {

HttpStream::HttpStream(std::shared_ptr<Socket> sock)
    :SocketStream(sock) {
}

std::shared_ptr<HttpRequest> HttpStream::recvRequest() {
    std::shared_ptr<HttpRequestParser> parser(new HttpRequestParser);//创建一个http request parser
    uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
    std::shared_ptr<char> buffer(new char[buff_size], [](char* ptr){
                delete[] ptr;
            });
    char* data = buffer.get();//data指向buffer缓冲区首地址
    int offset = 0;//初始值未0,未上一轮解析后未完成解析的数据长度，这些未解析的数据会被execute方法放在data的首部
    do {
        int len = read(data + offset, buff_size - offset);
        if(len <= 0) {//读数据时，返回0表示对方关闭了，返回-1表示read出错
            close();
            return nullptr;
        }
        len += offset;//当前len为这一轮应该解析的数据
        size_t nparse = parser->execute(data, len);
        if(parser->hasError()) {//解析出错，直接关闭连接
            close();
            return nullptr;
        }
        offset = len - nparse;
        if(offset == (int)buff_size) {
            close();
            return nullptr;
        }
        if(parser->isFinished()) {//如果解析成功，说明数据已经全部收到，跳出循环
            break;
        }
    } while(true);
    
    int64_t length = parser->getContentLength();
    if(length > 0) {
        std::string body;//用于存放消息体
        body.resize(length);

        int len = 0;
        if(length >= offset) {//表示消息体还没有接收完
            memcpy(&body[0], data, offset);
            len = offset;//len此时未body中放置的消息体数据的长度
        } else {
            memcpy(&body[0], data, length);
            len = length;//len此时未body中放置的消息体数据的长度
        }
        length -= offset;//结果为还需要获得的消息的长度
        if(length > 0) {
            if(readFixSize(&body[len], length) <= 0) {
                close();
                return nullptr;
            }
        }
        parser->getData()->setBody(body);//设置request报文的消息体
    }

    parser->getData()->init();//用于设置HttpRequest对象中的m_close参数
    return parser->getData();
}

int HttpStream::sendResponse(std::shared_ptr<HttpResponse> rsp) {
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());//将固定大小的数据发送出去
}

}

}