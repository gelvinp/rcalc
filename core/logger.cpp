#include "core/logger.h"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>

namespace RCalc {


StdLogger logger_instance;
Logger* Logger::global_logger = &logger_instance;


void Logger::configure(Mode new_mode) { global_logger->mode = new_mode; }


// These functions collect varargs into a va_list
void Logger::log(const char* p_format, ...)
{
    va_list args;
    va_start(args, p_format);
    global_logger->logf(LOG_STANDARD, p_format, args);
}

void Logger::log_err(const char* p_format, ...)
{
    va_list args;
    va_start(args, p_format);
    global_logger->logf(LOG_ERROR, p_format, args);
}

void Logger::log_info(const char* p_format, ...)
{
    va_list args;
    va_start(args, p_format);
    global_logger->logf(LOG_VERBOSE, p_format, args);
}


// If the set mode permits, renders the log message and passes it to the active logger
void Logger::logf(Mode msg_mode, const char* p_format, va_list args)
{
    if (msg_mode > mode) { return; }

    va_list args2;
    va_copy(args2, args);

    int buf_size = vsnprintf(nullptr, 0, p_format, args);
    va_end(args);

    std::vector<char> buf;
    buf.resize(buf_size + 1);

    vsnprintf(buf.data(), buf_size + 1, p_format, args2);
    va_end(args2);

    std::stringstream ss;

    switch (msg_mode)
    {
        case LOG_ERROR:
            ss << "[ERROR] ";
            break;
        case LOG_STANDARD:
            ss << "[LOG]   ";
            break;
        case LOG_VERBOSE:
            ss << "[INFO]  ";
            break;
    }

    ss << buf.data();

    logv(ss.str());
}


void StdLogger::logv(std::string message)
{
    std::cout << message << "\n";
}

}