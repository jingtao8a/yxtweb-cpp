/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-25 09:59:38
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 10:46:24
 * @FilePath: /YXTWebCpp/yxtwebcpp/config.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "config.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include "log.hpp"
#include "env.hpp"

namespace YXTWebCpp{
    static std::shared_ptr<Logger> logger = YXTWebCpp_LOG_NAME("system");

    std::shared_ptr<ConfigVarBase> Config::LookupBase(const std::string& name) {
        ReadScopedLockImpl<RWMutex> guard(getMutex());
        auto it = getDatas().find(name);
        if (it == getDatas().end()) {
            return nullptr;
        }
        return it->second;
    }

    void ListAllMember(const std::string& prefix, const YAML::Node& root, std::list<std::pair<std::string, const YAML::Node> >& all_nodes) {
        if(prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos) {
            YXTWebCpp_LOG_ERROR(logger) << "Config invalid name: " << prefix << " : " << root;
            return;
        }
        all_nodes.push_back(std::make_pair(prefix, root));
        if(root.IsMap()) {//只有当node是Map类型时
            for(auto it = root.begin();it != root.end(); ++it) {
                ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, all_nodes);
            }
        }
    }

    void Config::LoadFromYaml(const YAML::Node& root) {//通过此函数来获取配置文件，然后接着给对应的ConfigVar设置m_value
        std::list<std::pair<std::string, const YAML::Node> > all_nodes;
        ListAllMember("", root, all_nodes);

        for(auto& i : all_nodes) {
            std::string key = i.first;
            if(key.empty()) {
                continue;
            }
            //只有root的node是Map类型时，函数才会执行到这里
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);//将key变为小写
            std::shared_ptr<ConfigVarBase> var = LookupBase(key);//根据key的值找到对应的ConfigVar变量

            if(var) {
                if(i.second.IsScalar()) {//判断该all_nodes中的子node中是否为标量类型
                    var->fromString(i.second.Scalar());
                } else {//如果不是标量类型
                    //先将该node通过string流转换为std::string
                    std::stringstream ss;
                    ss << i.second;
                    //然后再将std::string转换为T
                    var->fromString(ss.str());
                }
            }
        }
    }

    static std::map<std::string, uint64_t> s_file2modifytime;
    static Mutex s_mutex;
/*
    void Config::LoadFromConfDir(const std::string& path, bool force) {
        std::string absoulte_path = YXTWebCpp::EnvMgr::GetInstance()->getAbsolutePath(path);
        std::vector<std::string> files;//用于存储对应目录下所有的yml文件的文件名
        FSUtil::ListAllFile(files, absoulte_path, ".yml");

        for(auto& i : files) {
            {
                struct stat st;
                lstat(i.c_str(), &st);
                std::lock_guard<std::mutex> lock(s_mutex);
                if(!force && s_file2modifytime[i] == (uint64_t)st.st_mtime) {
                    continue;
                }
                s_file2modifytime[i] = st.st_mtime;
            }
            try {
                YAML::Node root = YAML::LoadFile(i);
                LoadFromYaml(root);
            } catch (...) {
            }
        }
    }
*/
    void Config::Visit(std::function<void(std::shared_ptr<ConfigVarBase>)> cb) {
        ReadScopedLockImpl<RWMutex> guard(getMutex());
        std::map<std::string, std::shared_ptr<ConfigVarBase>>& m = getDatas();
        for(auto it = m.begin(); it != m.end(); ++it) {
            cb(it->second);
        }

    }
}