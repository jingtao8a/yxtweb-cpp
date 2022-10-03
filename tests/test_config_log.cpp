/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-25 16:17:39
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 10:54:32
 * @FilePath: /YXTWebCpp/tests/test_config_log.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../yxtwebcpp/yxtwebcpp.hpp"

int main() {
    YAML::Node node = YAML::LoadFile("../tests/logs.yml");
    YXTWebCpp::Config::LoadFromYaml(node);
    //YXTWebCpp_LOG_DEBUG(YXTWebCpp_LOG_NAME("root")) << "修改加载logs.yml配置的方式";
    std::cout << YXTWebCpp::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    YXTWebCpp_LOG_DEBUG(YXTWebCpp_LOG_NAME("system")) << "hello";
    return 0;
}
