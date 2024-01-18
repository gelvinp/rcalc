#pragma once

#include <cstdarg>
#include <fstream>
#include <string>
#include <string_view>

#include "core/logger.h"
#include "core/memory/cowvec.h"

namespace RCalc {

// Logs to std::cout
class StdOutLogger : public Logging::Engine
{
public:
    virtual ~StdOutLogger() = default;

    virtual void set_min_severity(Logging::Severity severity) override { min_severity = severity; }
    virtual void log(Logging::Severity severity, std::string message) override;

private:
    Logging::Severity min_severity;
};

// Logs to a file
class FileLogger : public Logging::Engine
{
public:
    FileLogger() = default;
    virtual ~FileLogger() = default;

    FileLogger(const FileLogger&) = delete;
    FileLogger(FileLogger&&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;
    FileLogger& operator=(FileLogger&&) = delete;
    
    bool open_file(std::string_view path);
    virtual void set_min_severity(Logging::Severity severity) override { min_severity = severity; }
    virtual void log(Logging::Severity severity, std::string message) override;

private:
    Logging::Severity min_severity;
    std::ofstream file;
    int write_count = 0;
};


// Logs to multiple sources
class CompoundLogger : public Logging::Engine
{
public:
    CompoundLogger() = default;
    virtual ~CompoundLogger() = default;

    CompoundLogger(const CompoundLogger&) = delete;
    CompoundLogger(CompoundLogger&&) = delete;
    CompoundLogger& operator=(const CompoundLogger&) = delete;
    CompoundLogger& operator=(CompoundLogger&&) = delete;

    void add_engine(std::shared_ptr<Logging::Engine> engine);
    virtual void set_min_severity(Logging::Severity severity) override;
    virtual void log(Logging::Severity severity, std::string message) override;

private:
    CowVec<std::shared_ptr<Logging::Engine>> engines;
};

}