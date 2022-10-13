/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-13 15:26:59
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-13 18:24:01
 * @FilePath: /yxtweb-cpp/tests/testB.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <iostream>
int sockfd = -1;

int main() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int rt = connect(sockfd, (sockaddr *)&addr, sizeof(addr));
    if (rt >= 0) {
        std::cout << "connect";
    } else {
        std::cout << "rt = " << rt << " errno = " << errno;
    }
    close(sockfd);
    return 0;
}