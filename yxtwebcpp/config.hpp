/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-18 22:34:14
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-05 23:02:53
 * @FilePath: /YXTWebCpp/yxtwebcpp/config.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include "mutex.hpp"

namespace YXTWebCpp {

class ConfigVarBase {
    public:
        ConfigVarBase(const std::string& name, const std::string& description = ""): m_name(name), m_description(description) {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);//将m_name转换为小写
        }
        virtual ~ConfigVarBase() {}

        const std::string& getName() const { return m_name;}
        
        const std::string& getDescription() const { return m_description;}

        //interface
        virtual std::string toString() = 0;
        virtual bool fromString(const std::string& val) = 0;
        //virtual std::string getTypeName() const = 0;

    private:
        std::string m_name;
        std::string m_description;
};

template<class F, class T>
class LexicalCast {
public:
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};
//偏特化
template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator()(const std::string& v) {//v是YAML string
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::list<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::set<T>)
 */
template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief 类型转换模板类片特化(std::set<T> 转换成 YAML String)
 */
template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_set<T>)
 */
template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief 类型转换模板类片特化(std::unordered_set<T> 转换成 YAML String)
 */
template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::map<std::string, T>)
 */
template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it) {//此时node是一个字典
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),
                        LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

/**
 * @brief 类型转换模板类片特化(std::map<std::string, T> 转换成 YAML String)
 */
template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));//设置node的锚点
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::unordered_map<std::string, T>)
 */
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),
                        LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

/**
 * @brief 类型转换模板类片特化(std::unordered_map<std::string, T> 转换成 YAML String)
 */
template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/*template<class T>
const char* TypeToName() {//不可重入函数
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}
*/
template<class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase {
    public:
        typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;//函数对象类型
        //构造函数中的default_value传值就是用来推断类型 T 的
        ConfigVar(const std::string& name, const T& default_value, const std::string& description = "")
                :ConfigVarBase(name, description), m_value(default_value) {}

        std::string toString() override { //将T类型的value转为string返回
            try {
                ReadScopedLockImpl<RWMutex> guard(m_mutex);
                return ToStr()(m_value);
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
            }
            return "";
        }

        bool fromString(const std::string& val) override {//将传入的参数val(string) 转为 T类型
            try {
                setValue(FromStr()(val));
                return true;
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
            }
            return false;
        }
/*
        std::string getTypeName() const override { 
            return TypeToName<T>();
        }
*/
        void setValue(const T& v) {//每次改变m_val时要通知所有的观察者
            {
                ReadScopedLockImpl<RWMutex> guard(m_mutex);
                if (m_value == v) {
                    return;
                }
                for (auto& i : m_cbs) {
                    i.second(m_value, v);//观察者模式
                }
            }
            WriteScopedLockImpl<RWMutex> guard(m_mutex);
            m_value = v;
        }

        const T getValue() {
            ReadScopedLockImpl<RWMutex> guard(m_mutex);
            return m_value;
        }
        
        uint64_t addListener(on_change_cb cb) {//添加观察者
            static uint64_t s_fun_id = 0;
            WriteScopedLockImpl<RWMutex> guard(m_mutex);
            ++s_fun_id;
            m_cbs[s_fun_id] = cb;
            return s_fun_id;
        }

        void delListener(uint64_t key) {//删除某个观察者
            WriteScopedLockImpl<RWMutex> guard(m_mutex);
            m_cbs.erase(key);
        }

        void clearListener() {//清空所有的观察者
            WriteScopedLockImpl<RWMutex> guard(m_mutex);
            m_cbs.clear();
        }
    private:
        RWMutex m_mutex;
        T m_value;
        std::map<uint64_t, on_change_cb> m_cbs;//用于设置观察者
};

class Config {
    public:
        template<class T>
        static std::shared_ptr<ConfigVar<T>> Lookup(const std::string& name, const T& default_value, const std::string description = "") {
            {
                ReadScopedLockImpl<RWMutex> guard(getMutex());
                auto it = getDatas().find(name);
                if (it != getDatas().end()) {//如果该ConfigVar变量存在
                    auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
                    return tmp;
                }
                if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos) {
                    throw std::invalid_argument(name);
                }
            }
            std::shared_ptr<ConfigVar<T> > v(new ConfigVar<T>(name, default_value, description));//没有的话就新建一个ConfigVar对象
            ReadScopedLockImpl<RWMutex> guard(getMutex());
            getDatas()[name] = v;//存放在map中
            return v;
        }

        template<class T>
        static std::shared_ptr<ConfigVar<T>> Lookup(const std::string& name) {
            ReadScopedLockImpl<RWMutex> guard(getMutex());
            auto it = getDatas().find(name);
            if (it == getDatas().end()) {
                return nullptr;
            }
            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }
        
        static std::shared_ptr<ConfigVarBase> LookupBase(const std::string& name);//返回指向ConfigVarBase对象的shared_ptr
        static void LoadFromYaml(const YAML::Node& root);
        //static void LoadFromConfDir(const std::string& path, bool force = false);
        static void Visit(std::function<void(std::shared_ptr<ConfigVarBase>)> cb);

    private:
        static std::map<std::string, std::shared_ptr<ConfigVarBase>>& getDatas() {
            static std::map<std::string, std::shared_ptr<ConfigVarBase>> s_datas;
            return s_datas;
        }

        static RWMutex& getMutex() {
            static RWMutex s_mutex;
            return s_mutex;
        }
};

}

#endif