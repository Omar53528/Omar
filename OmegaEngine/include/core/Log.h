#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace Omega {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5
};

class Log {
public:
    static void init(LogLevel level = LogLevel::Info);
    static void shutdown();
    
    static void trace(const std::string& message);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    static void fatal(const std::string& message);
    
    static void setLogFile(const std::string& filename);
    static void setConsoleOutput(bool enabled);
    
private:
    static void log(LogLevel level, const std::string& message);
    static std::string getTimestamp();
    static std::string levelToString(LogLevel level);
    
    static LogLevel s_minLevel;
    static bool s_consoleOutput;
    static std::ofstream s_logFile;
    static bool s_initialized;
};

} // namespace Omega
