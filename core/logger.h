#pragma once

#include <cstdarg>
#include <fstream>
#include <string>
#include <string_view>
#include <memory>

namespace RCalc {

namespace Logging {

enum Severity
{
    LOG_VERBOSE = 0,
    LOG_STANDARD = 1,
    LOG_ERROR = 2
};

class Engine {
public:
    virtual void set_min_severity(Severity severity) = 0;
    virtual void log(Severity severity, std::string message) = 0;
};

}


class Logger
{
public:
    virtual ~Logger() = default;
    
    static std::shared_ptr<Logging::Engine> get_global_engine() { return global_engine; }
    static void set_global_engine(std::shared_ptr<Logging::Engine> engine) { global_engine = engine; }

    static void log(const char* p_format, ...);
    static void log_err(const char* p_format, ...);
    static void log_info(const char* p_format, ...);

private:
    static std::shared_ptr<Logging::Engine> global_engine;

    static void logf(Logging::Severity severity, const char* p_format, va_list args);
};

}