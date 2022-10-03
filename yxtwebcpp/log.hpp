/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-09-17 15:47:18
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 10:16:24
 * @FilePath: /YXTWebCpp/yxtwebcpp/log.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include "singleton.hpp"
#include "util.hpp"
#include "thread.hpp"


//使用logger写入日志级别为level的日志（流式日志）
#define YXTWebCpp_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        YXTWebCpp::LogEventWrap(std::shared_ptr<YXTWebCpp::LogEvent>(new YXTWebCpp::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, YXTWebCpp::GetThreadId(),\
                YXTWebCpp::GetFiberId(), time(0), YXTWebCpp::Thread::GetName()))).getSS()

//使用logger写入日志界别为debug的日志（流式日志）
#define YXTWebCpp_LOG_DEBUG(logger) YXTWebCpp_LOG_LEVEL(logger, YXTWebCpp::LogLevel::DEBUG)
//使用logger写入日志界别为info的日志（流式日志）
#define YXTWebCpp_LOG_INFO(logger) YXTWebCpp_LOG_LEVEL(logger, YXTWebCpp::LogLevel::INFO)
//使用logger写入日志界别为warn的日志（流式日志）
#define YXTWebCpp_LOG_WARN(logger) YXTWebCpp_LOG_LEVEL(logger, YXTWebCpp::LogLevel::WARN)
//使用logger写入日志界别为error的日志（流式日志）
#define YXTWebCpp_LOG_ERROR(logger) YXTWebCpp_LOG_LEVEL(logger, YXTWebCpp::LogLevel::ERROR)
//使用logger写入日志界别为fatal的日志（流式日志）
#define YXTWebCpp_LOG_FATAL(logger) YXTWebCpp_LOG_LEVEL(logger, YXTWebCpp::LogLevel::FATAL)

//使用logger写入日志级别为level的日志（格式化,printf）
#define YXTWebCpp_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        YXTWebCpp::LogEventWrap(std::shared_ptr<YXTWebCpp::LogEvent>(new YXTWebCpp::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, YXTWebCpp::GetThreadId(),\
                YXTWebCpp::GetFiberId(), time(0), YXTWebCpp::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

//使用logger写入日志界别为debug的日志（格式化,printf）
#define YXTWebCpp_LOG_FMT_DEBUG(logger, fmt, ...) YXTWebCpp_LOG_FMT_LEVEL(logger, YXTWebCpp::LogLevel::DEBUG, fmt, __VA_ARGS__)
//使用logger写入日志界别为info的日志（格式化,printf）
#define YXTWebCpp_LOG_FMT_INFO(logger, fmt, ...)  YXTWebCpp_LOG_FMT_LEVEL(logger, YXTWebCpp::LogLevel::INFO, fmt, __VA_ARGS__)
//使用logger写入日志界别为warn的日志（格式化,printf）
#define YXTWebCpp_LOG_FMT_WARN(logger, fmt, ...)  YXTWebCpp_LOG_FMT_LEVEL(logger, YXTWebCpp::LogLevel::WARN, fmt, __VA_ARGS__)
//使用logger写入日志界别为error的日志（格式化,printf）
#define YXTWebCpp_LOG_FMT_ERROR(logger, fmt, ...) YXTWebCpp_LOG_FMT_LEVEL(logger, YXTWebCpp::LogLevel::ERROR, fmt, __VA_ARGS__)
//使用logger写入日志界别为fatal的日志（格式化,printf）
#define YXTWebCpp_LOG_FMT_FATAL(logger, fmt, ...) YXTWebCpp_LOG_FMT_LEVEL(logger, YXTWebCpp::LogLevel::FATAL, fmt, __VA_ARGS__)

//获取主日志器
#define YXTWebCpp_LOG_ROOT() YXTWebCpp::LoggerMgr::GetInstance()->getRoot()

//获取指定名称的日志器，如果不存在则创建
#define YXTWebCpp_LOG_NAME(name) YXTWebCpp::LoggerMgr::GetInstance()->getLogger(name)

namespace YXTWebCpp{
    
class Logger;

class LogLevel {
public:
    enum Level {
        /// 未知级别
        UNKNOW = 0,
        /// DEBUG 级别
        DEBUG = 1,
        /// INFO 级别
        INFO = 2,
        /// WARN 级别
        WARN = 3,
        /// ERROR 级别
        ERROR = 4,
        /// FATAL 级别
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
    
    static LogLevel::Level FromString(const std::string& str);
};

class LogEvent {
    public:
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name);
        
        const char* getFile() const { return m_file;}

        int32_t getLine() const { return m_line;}

        uint32_t getElapse() const { return m_elapse;}

        uint32_t getThreadId() const { return m_threadId;}

        uint32_t getFiberId() const { return m_fiberId;}

        uint64_t getTime() const { return m_time;}

        const std::string& getThreadName() const { return m_threadName;}

        std::string getContent() const { return m_ss.str();}

        std::shared_ptr<Logger> getLogger() const { return m_logger;}

        LogLevel::Level getLevel() const { return m_level;}

        std::stringstream& getSS() { return m_ss;}

        void format(const char* fmt, ...);

        void format(const char* fmt, va_list al);

    private:
        /// 文件名
        const char* m_file = nullptr;
        /// 行号
        int32_t m_line = 0;
        /// 程序启动开始到现在的毫秒数
        uint32_t m_elapse = 0;
        /// 线程ID
        uint32_t m_threadId = 0;
        /// 协程ID
        uint32_t m_fiberId = 0;
        /// 时间戳
        uint64_t m_time = 0;
        /// 线程名称
        std::string m_threadName;
        /// 日志内容流
        std::stringstream m_ss;
        /// 日志器
        std::shared_ptr<Logger> m_logger;
        /// 日志等级
        LogLevel::Level m_level;
};

//适配器模式RAII
class LogEventWrap {
public:
    LogEventWrap(std::shared_ptr<LogEvent> e);

    ~LogEventWrap();
    
    std::shared_ptr<LogEvent> getEvent() const { return m_event;}
    
    std::stringstream& getSS() { return m_event->getSS(); }

private:
    std::shared_ptr<LogEvent> m_event;
};

class LogFormatter {
    public:
        /**
         * @brief 构造函数
         * @param[in] pattern 格式模板
         * @details 
         *  %m 消息
         *  %p 日志级别
         *  %r 累计毫秒数
         *  %c 日志名称
         *  %t 线程id
         *  %n 换行
         *  %d 时间
         *  %f 文件名
         *  %l 行号
         *  %T 制表符
         *  %F 协程id
         *  %N 线程名称
         *
         *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
         */
        LogFormatter(const std::string& pattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");

        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event);

        std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event);
        
        std::string getPattern() { return m_pattern; }

        virtual ~LogFormatter() {}

        bool isError() const { return m_error;}

    public:
        class FormatItem {
            public:
                virtual ~FormatItem() {}
                virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) = 0;
        };

    private:    
        void init();
    private:
        /// 日志格式模板
        std::string m_pattern;
        /// 日志格式解析后格式
        std::vector<std::shared_ptr<FormatItem> > m_items;
        /// 格式是否有错误
        bool m_error = false;
};

class LogAppender {
    friend class Logger;
    public:
        virtual ~LogAppender() {}

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) = 0;

        virtual std::string toYamlString() = 0;

        //m_formatter
        void setFormatter(std::shared_ptr<LogFormatter> val) {
            ScopedLockImpl<Mutex> guard(m_mutex);
            m_formatter = val;
            if(m_formatter) {
                m_hasFormatter = true;
            } else {
                m_hasFormatter = false;
            }
        }

        std::shared_ptr<LogFormatter> getFormatter() {
            ScopedLockImpl<Mutex> guard(m_mutex);
            return m_formatter;
        }
        //m_level
        LogLevel::Level getLevel() const { return m_level;}

        void setLevel(LogLevel::Level val) { m_level = val; }

    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;

        bool m_hasFormatter = false;

        Mutex m_mutex;

        std::shared_ptr<LogFormatter> m_formatter;
};

class Logger : public std::enable_shared_from_this<Logger> {

friend class LoggerManager;

public:
    Logger(const std::string& name = "root");//默认构造root logger

    void log(LogLevel::Level level, std::shared_ptr<LogEvent> event);

    void debug(std::shared_ptr<LogEvent> event) {
        log(LogLevel::DEBUG, event);
    }

    void info(std::shared_ptr<LogEvent> event) {
        log(LogLevel::INFO, event);
    }

    void warn(std::shared_ptr<LogEvent> event) {
        log(LogLevel::WARN, event);
    }

    void error(std::shared_ptr<LogEvent> event) {
        log(LogLevel::ERROR, event);
    }

    void fatal(std::shared_ptr<LogEvent> event) {
        log(LogLevel::FATAL, event);
    }

    //m_appenders
    void addAppender(std::shared_ptr<LogAppender> appender);

    void delAppender(std::shared_ptr<LogAppender> appender);

    void clearAppenders();

    //m_level
    LogLevel::Level getLevel() const { return m_level;}

    void setLevel(LogLevel::Level val) { m_level = val;}
    
    //m_name
    const std::string& getName() const { return m_name;}
    
    //m_formatter
    void setFormatter(std::shared_ptr<LogFormatter> val);
    
    void setFormatter(const std::string& val);

    std::shared_ptr<LogFormatter> getFormatter();

    /**
     * @brief 将日志器的配置转成YAML String
     */
    std::string toYamlString();
    
private:
    /// 日志名称
    std::string m_name;
    /// 日志级别
    LogLevel::Level m_level;
    /// Mutex
    Mutex m_mutex;
    /// 日志目标集合
    std::list<std::shared_ptr<LogAppender> > m_appenders;
    /// 日志格式器
    std::shared_ptr<LogFormatter> m_formatter;
    /// 主日志器
    std::shared_ptr<Logger> m_root;
};

/**
 * @brief 日志器管理类
 */
class LoggerManager {
public:

    LoggerManager();

    std::shared_ptr<Logger> getLogger(const std::string& name);

    void init();

    std::shared_ptr<Logger> getRoot() const { return m_root;}

    /**
     * @brief 将所有的日志器配置转成YAML String
     */
    std::string toYamlString();

private:
    /// Mutex
    Mutex m_mutex;
    /// 日志器容器
    std::map<std::string, std::shared_ptr<Logger> > m_loggers;
    /// 主日志器
    std::shared_ptr<Logger> m_root;
};


/* @brief 输出到控制台的Appender
 */
class StdoutLogAppender : public LogAppender {
public:
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override;

    std::string toYamlString() override;
};


/**
 * @brief 输出到文件的Appender
 */
class FileLogAppender : public LogAppender {
public:
    FileLogAppender(const std::string& filename);
    
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override;

    std::string toYamlString() override;

    bool reopen();
private:
    /// 文件路径
    std::string m_filename;
    /// 文件流
    std::ofstream m_filestream;
    /// 上次重新打开时间
    uint64_t m_lastTime = 0;
};

typedef Singleton<LoggerManager> LoggerMgr;

}
#endif 