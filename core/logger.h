#pragma once

#include <cstdarg>
#include <string>


class Logger
{
public:
    enum Mode
    {
        LOG_ERROR = 0,
        LOG_STANDARD = 1,
        LOG_VERBOSE = 2
    };

    static void configure(Mode new_mode);

    static void log(const char* p_format, ...);
    static void log_err(const char* p_format, ...);
    static void log_info(const char* p_format, ...);

private:
    Mode mode;
    static Logger* global_logger;

    void logf(Mode mode, const char* p_format, va_list args);
    virtual void logv(std::string message) = 0;
};


// Logs to std::cout
class StdLogger : public Logger
{
    virtual void logv(std::string message) override;
};
