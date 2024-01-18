#include "engines.h"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>

namespace RCalc {
using namespace Logging;


std::shared_ptr<Logging::Engine> Logger::global_engine = nullptr;


// These functions collect varargs into a va_list
void Logger::log(const char* p_format, ...)
{
    if (!global_engine) { return; }
    va_list args;
    va_start(args, p_format);
    logf(LOG_STANDARD, p_format, args);
}

void Logger::log_err(const char* p_format, ...)
{
    if (!global_engine) { return; }
    va_list args;
    va_start(args, p_format);
    logf(LOG_ERROR, p_format, args);
}

void Logger::log_info(const char* p_format, ...)
{
    if (!global_engine) { return; }
    va_list args;
    va_start(args, p_format);
    logf(LOG_VERBOSE, p_format, args);
}


// If the set mode permits, renders the log message and passes it to the active logger
void Logger::logf(Logging::Severity severity, const char* p_format, va_list args)
{
    if (!global_engine) { return; }

    va_list args2;
    va_copy(args2, args);

    int buf_size = vsnprintf(nullptr, 0, p_format, args);
    va_end(args);

    std::string str;
    str.resize(buf_size + 1, 0);

    vsnprintf(str.data(), buf_size + 1, p_format, args2);
    va_end(args2);

    global_engine->log(severity, str);
}


void StdOutLogger::log(Severity severity, std::string message)
{
    if (severity < min_severity) { return; }

    switch (severity)
    {
        case LOG_ERROR:
            std::cout << "[ERROR] ";
            break;
        case LOG_STANDARD:
            std::cout << "[LOG]   ";
            break;
        case LOG_VERBOSE:
            std::cout << "[INFO]  ";
            break;
    }

    std::cout << message << "\n";
}


bool FileLogger::open_file(std::string_view path) {
    if (file.is_open()) {
        file.flush();
        file.close();
    }

    file.open(path.data(), file.out | file.trunc);
    return file.is_open();
}

void FileLogger::log(Severity severity, std::string message)
{
    if (severity < min_severity) { return; }
    if (!file.is_open()) { return; }

    switch (severity)
    {
        case LOG_ERROR:
            file << "[ERROR] ";
            break;
        case LOG_STANDARD:
            file << "[LOG]   ";
            break;
        case LOG_VERBOSE:
            file << "[INFO]  ";
            break;
    }

    file << message << "\n";

    if (++write_count > 10) {
        file.flush();
        write_count = 0;
    }
}


void CompoundLogger::add_engine(std::shared_ptr<Logging::Engine> engine) {
    engines.push_back(engine);
}

void CompoundLogger::set_min_severity(Severity severity) {
    for (const std::shared_ptr<Logging::Engine>& engine : engines) {
        engine->set_min_severity(severity);
    }
}

void CompoundLogger::log(Logging::Severity severity, std::string message) {
    for (const std::shared_ptr<Logging::Engine>& engine : engines) {
        engine->log(severity, message);
    }
}

}