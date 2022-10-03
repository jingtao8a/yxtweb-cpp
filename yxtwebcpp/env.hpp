/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-29 10:27:26
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-09-29 20:06:38
 * @FilePath: /yxtweb-cpp/yxtwebcpp/env.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _ENV_HPP_
#define _ENV_HPP_

#include <string>
#include <map>
#include <vector>
#include "singleton.hpp"
#include "mutex.hpp"


namespace YXTWebCpp {

class  Env{
public:
    bool init(int argc, char** argv);
    void add(const std::string&key, const std::string& val);
    bool has(const std::string& key);
    void del(const std::string& key);
    std::string get(const std::string& key, const std::string& default_value = "");

    void addHelp(const std::string& key, const std::string& desc);
    void removeHelp(const std::string& key);
    void printHelp();

    const std::string& getExe() const { return m_exe; }
    const std::string& getCwd() const { return m_cwd; }

    bool setEnv(const std::string& key, const std::string& val);
    std::string getEnv(const std::string& key, const std::string& default_value = "");

    std::string getAbsolutePath(const std::string& path) const;
    std::string getAbsoluteWorkPath(const std::string& path) const;
    std::string getConfigPath();
private:
    std::map<std::string, std::string> m_args;
    std::vector<std::pair<std::string, std::string> > m_helps;
    std::string m_exe;
    std::string m_cwd;
    std::string m_program;
    Mutex m_mutex;
};

typedef Singleton<Env> EnvMgr;

}

#endif