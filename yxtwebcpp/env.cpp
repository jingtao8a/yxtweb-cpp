#include "env.hpp"
#include "log.hpp"
#include "config.hpp"
#include <memory>
#include <unistd.h>
#include <iostream>

namespace YXTWebCpp{
    static std::shared_ptr<Logger> g_logger = YXTWebCpp_LOG_NAME("system");

    bool Env::init(int argc, char** argv) {
        char link[1024] = {0};
        char path[1024] = {0};
        sprintf(link, "/proc/%d/exe", getpid());
        readlink(link, path, sizeof(path));
        
        m_exe = path;//实际运行程序的符号连接
        auto pos = m_exe.find_last_of("/");
        m_cwd = m_exe.substr(0, pos) + "/";//实际运行程序的路径

        m_program = argv[0];

        const char* now_key = nullptr;
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-') {
                if (strlen(argv[i]) > 1) {
                    if (now_key) {
                        add(now_key, "");
                    }
                    now_key = argv[i] + 1;
                } else {
                    //YXTWebCpp_LOG_ERROR(g_lgger) << "invalid arg idx=" << i << "val = " << argv[i];
                    return false;
                }
            } else {
                if (now_key) {
                    add(now_key, argv[i]);
                    now_key = nullptr;
                } else {
                    //YXTWebCpp_LOG_ERROR(g_logger)
                    return false;
                }
            }
        }
        if (now_key) {
            add(now_key, "");
        }
        return true;
    }

    void Env::add(const std::string& key, const std::string& val) {
        ScopedLockImpl<Mutex> guard(m_mutex);
        m_args[key] = val;
    }

    bool Env::has(const std::string& key) {
        ScopedLockImpl<Mutex> guard(m_mutex);
        auto it = m_args.find(key);
        return it != m_args.end();
    }

    void Env::del(const std::string& key) {
        ScopedLockImpl<Mutex> guard(m_mutex);
        m_args.erase(key);
    }

    std::string Env::get(const std::string& key, const std::string& default_value) {
        ScopedLockImpl<Mutex> guard(m_mutex);
        auto it = m_args.find(key);
        return it != m_args.end() ? it->second : default_value;
    }

    void Env::addHelp(const std::string& key, const std::string& desc) {
        removeHelp(key);
        ScopedLockImpl<Mutex> guard(m_mutex);
        m_helps.push_back(std::make_pair(key, desc));
    }

    void Env::removeHelp(const std::string& key) {
        ScopedLockImpl<Mutex> guard(m_mutex);
        for(auto it = m_helps.begin(); it != m_helps.end();) {
            if(it->first == key) {
                it = m_helps.erase(it);
            } else {
                ++it;
            }
        }
    }

    void Env::printHelp() {
        ScopedLockImpl<Mutex> guard(m_mutex);
        std::cout << "Usage: " << m_program << " [options]" << std::endl;
        for(auto& i : m_helps) {
            std::cout <<  "-" << i.first << " : " << i.second << std::endl;
        }
    }
    
    bool Env::setEnv(const std::string& key, const std::string& val) {
        return !setenv(key.c_str(), val.c_str(), 1);//设置本进程运行时环境变量
    }

    std::string Env::getEnv(const std::string& key, const std::string& default_value) {
        const char* v = getenv(key.c_str());//获取本进程运行时环境变量
        if(v == nullptr) {
            return default_value;
        }
        return v;
    }

    std::string Env::getAbsolutePath(const std::string& path) const {
        if(path.empty()) {
            return "/";
        }
        if(path[0] == '/') {
            return path;
        }
        return m_cwd + path;
    }

    std::string Env::getAbsoluteWorkPath(const std::string& path) const {
        if(path.empty()) {
            return "/";
        }
        if(path[0] == '/') {
            return path;
        }
        static std::shared_ptr<ConfigVar<std::string> > g_server_work_path = Config::Lookup<std::string>("server.work_path");
        return g_server_work_path->getValue() + "/" + path;
    }

    std::string Env::getConfigPath() {
        return getAbsolutePath(get("c", "conf"));
    }

}   