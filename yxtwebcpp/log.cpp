/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-17 15:47:12
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-09-29 20:03:54
 * @FilePath: /YXTWebCpp/yxtwebcpp/log.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE <
 */
#include "log.hpp"
#include <stdarg.h>
#include <iostream>
#include <functional>
#include <yaml-cpp/yaml.h>
#include "config.hpp"
#include "env.hpp"

namespace YXTWebCpp{

//LogLevel
const char* LogLevel::ToString(LogLevel::Level level) {
#define XX(name) \
        case LogLevel::name:\
            return #name;\
            break;
        
    switch (level) {
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
        default:
            return "UNKNOW";
    }

#undef XX

}

LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(level, v) \
        if (str == #v) {\
            return LogLevel::level;\
        }
        
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal)

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;

#undef XX
    
}

//LogEvent
LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
        ,const char* file, int32_t line, uint32_t elapse
        ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
        ,const std::string& thread_name)     
        :m_file(file)
        ,m_line(line)
        ,m_elapse(elapse)
        ,m_threadId(thread_id)
        ,m_fiberId(fiber_id)
        ,m_time(time)
        ,m_threadName(thread_name)
        ,m_logger(logger)
        ,m_level(level) {

}

void LogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1) {
        m_ss << std::string(buf, len);
        free(buf);
    }
}


//LogEventWrap
LogEventWrap::LogEventWrap(std::shared_ptr<LogEvent> e) :m_event(e) {}

LogEventWrap::~LogEventWrap() {
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

//LogFormatter::FormatItem
class MessageFormatItem : public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << event->getFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << event->getThreadName();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") :m_format(format) {}

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << std::endl;
    }
};


class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str) :m_string(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override {
        os << "\t";
    }
};



//LogFormatter pattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
LogFormatter::LogFormatter(const std::string& pattern) :m_pattern(pattern) {
    init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) {
    std::stringstream ss;
    for(auto& i : m_items) {//通过每个LofFormatter::FormatItem依次
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) {
    for(auto& i : m_items) {
        i->format(ofs, logger, level, event);
    }
    return ofs;
}
//"%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
void LogFormatter::init() {
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        //当m_pattern[i] != '%'时
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }
        //当m_pattern[i] == '%' 且 m_pattern[i + 1] == '%'时，即%%
        if(i + 1 < m_pattern.size() && m_pattern[i + 1] == '%') {
            nstr.append(1, '%');
            continue;
        }
        //当m_pattern[i] == '%' 且 m_pattern[i + 1] != '%'时
        int fmt_status = 0;//为1表示解析格式，遇到{进入解析模式
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        size_t n = i + 1;
        while(n < m_pattern.size()) {
            if(fmt_status == 0) {
                if (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}') {//非英文字符且除了{和}的其它字符即[ 、] 、 :、%
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                } else if (m_pattern[n] == '{') {//这里为{
                    str = m_pattern.substr(i + 1, n - i - 1);
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {//结束解析模式
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;//格式错误
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<std::shared_ptr<LogFormatter::FormatItem>(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return std::shared_ptr<LogFormatter::FormatItem>(new C(fmt));}}

        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(T, TabFormatItem),               //T:Tab
        XX(F, FiberIdFormatItem),           //F:协程id
        XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(std::shared_ptr<LogFormatter::FormatItem>(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(std::shared_ptr<LogFormatter::FormatItem>(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;//格式错误
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
    }
}

//Logger
Logger::Logger(const std::string& name) :m_name(name) ,m_level(LogLevel::DEBUG) {//默认名字是root，默认logger级别是DEBUG
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::setFormatter(std::shared_ptr<LogFormatter> val) {
    ScopedLockImpl<Mutex> guard(m_mutex);
    m_formatter = val;
    for(auto& i : m_appenders) {
        ScopedLockImpl<Mutex> g(i->m_mutex);
        if(!i->m_hasFormatter) {//如果该logger的appender没有m_formatter,设置为logger的m_formatter
            i->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string& val) {//重载
    std::cout << "---" << val << std::endl;
    std::shared_ptr<LogFormatter> new_val(new LogFormatter(val));
    if(new_val->isError()) {
        std::cout << "Logger setFormatter name=" << m_name
                << " value=" << val << " invalid formatter"
                << std::endl;
        return;
    }
    setFormatter(new_val);
}

std::shared_ptr<LogFormatter> Logger::getFormatter() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    return m_formatter;
}

std::string Logger::toYamlString() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }

    for(auto& i : m_appenders) {
        node["appenders"].push_back(YAML::Load(i->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

void Logger::addAppender(std::shared_ptr<LogAppender> appender) {
    ScopedLockImpl<Mutex> guard(m_mutex);
    if(!appender->getFormatter()) {//如果appender没有formatter，使用日志器的formatter
        ScopedLockImpl<Mutex> g(appender->m_mutex);
        appender->m_formatter = m_formatter;
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(std::shared_ptr<LogAppender> appender) {
    ScopedLockImpl<Mutex> guard(m_mutex);
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if(*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    m_appenders.clear();
}

void Logger::log(LogLevel::Level level, std::shared_ptr<LogEvent> event) {
    if(level >= m_level) {//只有event的level大于logger的level，日志器才会执行
        auto self = shared_from_this();//指向this的shared_ptr
        ScopedLockImpl<Mutex> guard(m_mutex);//表示这个日志器已经被一个线程占用了
        if(!m_appenders.empty()) {//每个logAppender调用log
            for(auto& i : m_appenders) {
                i->log(self, level, event);
            }
        } else if(m_root) {//主logger调用log
            m_root->log(level, event);
        }
    }
}
//FileLogAppender
FileLogAppender::FileLogAppender(const std::string& filename): m_filename(filename) {
    reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) {
    if(level >= m_level) {
        uint64_t now = event->getTime();
        if(now >= (m_lastTime + 3)) {//距离上次打开的时间超过3s,重新打开
            reopen();
            m_lastTime = now;
        }
        ScopedLockImpl<Mutex> guard(m_mutex);
        if(!m_formatter->format(m_filestream, logger, level, event)) {
            std::cout << "error" << std::endl;
        }
    }
}

std::string FileLogAppender::toYamlString() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    YAML::Node node;
    node["type"] = "FileLogAppender";
    node["file"] = m_filename;
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

bool FileLogAppender::reopen() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    if(m_filestream) {//如果该流已经打开，关闭
        m_filestream.close();
    }
    m_filestream.open(m_filename.c_str(), std::ios::app);
    if (!m_filestream) {
        return false;
    } else {
        return true;
    }
}


//StdoutLogAppender 
void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) {
    if(level >= m_level) {
        ScopedLockImpl<Mutex> guard(m_mutex);
        m_formatter->format(std::cout, logger, level, event);
    }
}

std::string StdoutLogAppender::toYamlString() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

//LoggerManager

LoggerManager::LoggerManager() {
    m_root.reset(new Logger());//m_root指向一个默认日志器(root DEBUG)
    m_root->addAppender(std::shared_ptr<LogAppender>(new StdoutLogAppender));

    m_loggers[m_root->m_name] = m_root;
}

std::shared_ptr<Logger> LoggerManager::getLogger(const std::string& name) {
    ScopedLockImpl<Mutex> guard(m_mutex);//表示一个线程占用了这个LoggerMannger
    auto it = m_loggers.find(name);
    if(it != m_loggers.end()) {//如果找到这个logger，就返回这个logger
        return it->second;
    }

    std::shared_ptr<Logger> logger(new Logger(name));//否则新建一个logger并且返回,那么这个Logger的名字为name，级别为DEBUG
    logger->m_root = m_root;//设置新建的logger中的m_root等于m_root
    m_loggers[name] = logger;
    return logger;
}


std::string LoggerManager::toYamlString() {
    ScopedLockImpl<Mutex> guard(m_mutex);
    YAML::Node node;
    for(auto& i : m_loggers) {
        node.push_back(YAML::Load(i.second->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}


struct LogAppenderDefine {//LogAppender配置定义
    int type = 0; //1 File, 2 Stdout
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {//==运算符重载用于判断两个LogAppender配置定义对象是否相等
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine {//Log配置定义
    std::string name;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;//该Log配置定义下的一组LogAppender配置定义

    bool operator==(const LogDefine& oth) const {//判断两个Log配置定义对象是否相等
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == appenders;
    }

    bool operator<(const LogDefine& oth) const {//比较两个Log配置对象，这里采用字典序
        return name < oth.name;
    }

    bool isValid() const {//判断该Log配置定义是否合法（判断该Log配置的name是否为空）
        return !name.empty();
    }
};

//偏特化
template<>//将YAML string转为LogDefine
class LexicalCast<std::string, LogDefine> {
public:
    LogDefine operator()(const std::string& v) {
        YAML::Node n = YAML::Load(v);//取出YAML node
        LogDefine ld;//定义一个LogDefine ld
        if(!n["name"].IsDefined()) {
            std::cout << "log config error: name is null, " << n << std::endl;
            throw std::logic_error("log config name is null");
        }
        ld.name = n["name"].as<std::string>();//取出LogDefine的name
        ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");//取出LogDefine的level
        if(n["formatter"].IsDefined()) {//判断formaterr是否存在
            ld.formatter = n["formatter"].as<std::string>();//存在的话直接取出
        }

        if(n["appenders"].IsDefined()) {//判断appenders是否存在
            for(size_t x = 0; x < n["appenders"].size(); ++x) {//遍历sequence类型的node
                auto a = n["appenders"][x];
                if(!a["type"].IsDefined()) {//如果appender的type未定义直接跳过，查找下一个appender
                    std::cout << "log config error: appender type is null, " << a << std::endl;
                    continue;
                }
                std::string type = a["type"].as<std::string>();//取得type
                LogAppenderDefine lad;
                if(type == "FileLogAppender") {
                    lad.type = 1;
                    if(!a["file"].IsDefined()) {//appender的file未定义直接跳过
                        std::cout << "log config error: fileappender file is null, " << a << std::endl;
                        continue;
                    }
                    lad.file = a["file"].as<std::string>();
                    if(a["formatter"].IsDefined()) {
                        lad.formatter = a["formatter"].as<std::string>();
                    }
                } else if(type == "StdoutLogAppender") {
                    lad.type = 2;
                    if(a["formatter"].IsDefined()) {
                        lad.formatter = a["formatter"].as<std::string>();
                    }
                } else {//如果type不是上述的两种类型，直接查看下一个appender
                    std::cout << "log config error: appender type is invalid, " << a << std::endl;
                    continue;
                }

                ld.appenders.push_back(lad);//将该LogDefine对象直接放在LogDefine的vector对象中
            }
        }
        return ld;//返回LogDfine对象
    }
};

template<>//将LogDefine 转为 YAML string
class LexicalCast<LogDefine, std::string> {
public:
    std::string operator()(const LogDefine& i) {
        YAML::Node n;//最外层的Node
        n["name"] = YAML::Load(i.name);
        if(i.level != LogLevel::UNKNOW) {
            n["level"] = YAML::Load(LogLevel::ToString(i.level));
        }
        if(!i.formatter.empty()) {
            n["formatter"] = YAML::Load(i.formatter);
        }

        for(auto& a : i.appenders) {//遍历该LogDefine的所有LogAppenderDefine
            YAML::Node na;//n的内层的Node
            if(a.type == 1) {
                na["type"] = YAML::Load("FileLogAppender");
                na["file"] = YAML::Load(a.file);
            } else if(a.type == 2) {
                na["type"] = YAML::Load("StdoutLogAppender");
            }
            if(a.level != LogLevel::UNKNOW) {
                na["level"] = LogLevel::ToString(a.level);
            }

            if(!a.formatter.empty()) {
                na["formatter"] = a.formatter;
            }

            n["appenders"].push_back(na);
        }
        std::stringstream ss;
        ss << n;
        return ss.str();
    }
};

std::shared_ptr<ConfigVar<std::set<LogDefine> > > g_log_defines = YXTWebCpp::Config::Lookup("logs", std::set<LogDefine>(), "logs config");
//default_value的类型 T 为std::set<LogDefine>

//Logger初始器
struct LogIniter {
    LogIniter() {
        //添加ConfigVar<std::set<LogDefine> >的观察者
        g_log_defines->addListener([](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value){
            for(auto& i : new_value) {
                auto it = old_value.find(i);//对于每个在new_val中的LogDefine判断在old_value中能否找到
                std::shared_ptr<Logger> logger;
                if(it == old_value.end()) {//如果没有找到
                    //新增logger
                    logger = YXTWebCpp_LOG_NAME(i.name);
                } else {
                        continue;
                }
                logger->setLevel(i.level);//设置LogLevel

                if(!i.formatter.empty()) {
                    logger->setFormatter(i.formatter);//如果有的话，设置formatter
                }

                logger->clearAppenders();//清空所有的appender
                for(auto& a : i.appenders) {
                    std::shared_ptr<LogAppender> ap;
                    if(a.type == 1) {
                        ap.reset(new FileLogAppender(a.file));
                    } else if(a.type == 2) {
                        if(!EnvMgr::GetInstance()->has("d")) {
                            ap.reset(new StdoutLogAppender);
                        } else {
                            continue;
                        }
                    }
                    ap->setLevel(a.level);
                    if(!a.formatter.empty()) {
                        std::shared_ptr<LogFormatter> fmt(new LogFormatter(a.formatter));
                        if(!fmt->isError()) {
                            ap->setFormatter(fmt);
                        } else {
                            std::cout << "log.name=" << i.name << " appender type=" << a.type
                                << " formatter=" << a.formatter << " is invalid" << std::endl;
                        }
                    }
                    logger->addAppender(ap);//如果appender的formatter为空时会给它加上
                }
            }

            for(auto& i : old_value) {
                auto it = new_value.find(i);//判断每个old_value中的LogDefine是否在new_value中
                if(it == new_value.end()) {//如果没找到
                    //删除logger
                    auto logger = YXTWebCpp_LOG_NAME(i.name);//找到对应的logger
                    logger->setLevel((LogLevel::Level)0);//level设置为UNKNOWN(0)，表示不再使用了
                    logger->clearAppenders();//删除所有的appenders
                }
            }
        });
    }
};

static LogIniter __log_init;

}