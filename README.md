sylar-framework，make wheels<br>
## usage
使用前需安装外部库yaml-cpp、boost
1. 安装yaml-cpp
```shell
git clone https://github.com/jbeder/yaml-cpp.git
mkdir build
cd build
cmake [-G generator] [-DYAML_BUILD_SHARED_LIBS=on|OFF] ..
sudo make install
```
2. 安装boost
```shell
sudo apt update
sudo apt install libboost-all-dev
```

---
参考博客: [从零开始重写sylar C++高性能分布式服务器框架](https://www.midlane.top/wiki/pages/viewpage.action?pageId=10060952)<br>
参考项目: [C++高性能分布式服务器框架](https://github.com/zhongluqiang/sylar-from-scratch)