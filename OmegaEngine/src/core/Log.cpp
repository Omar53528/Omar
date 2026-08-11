#include "core/Log.h"

namespace Omega {

LogLevel Log::s_minLevel = LogLevel::Info;
bool Log::s_consoleOutput = true;
std::ofstream Log::s_logFile;
bool Log::s_initialized = false;

void Log::init(LogLevel level) {
    if (s_initialized) return;
    
    s_minLevel = level;
    s_consoleOutput = true;
    s_initialized = true;
    
    info("Omega Engine Logging System initialized");
}

void Log::shutdown() {
    if (s_logFile.is_open()) {
        s_logFile.close();
    }
    s_initialized = false;
}

void Log::trace(const std::string& message) { log(LogLevel::Trace, message); }
void Log::debug(const std::string& message) { log(LogLevel::Debug, message); }
void Log::info(const std::string& message) { log(LogLevel::Info, message); }
void Log::warn(const std::string& message) { log(LogLevel::Warning, message); }
void Log::error(const std::string& message) { log(LogLevel::Error, message); }
void Log::fatal(const std::string& message) { log(LogLevel::Fatal, message); }

void Log::setLogFile(const std::string& filename) {
    if (s_logFile.is_open()) {
        s_logFile.close();
    }
    s_logFile.open(filename, std::ios::app);
    if (!s_logFile.is_open()) {
        error("Failed to open log file: " + filename);
    }
}

void Log::setConsoleOutput(bool enabled) {
    s_consoleOutput = enabled;
}

void Log::log(LogLevel level, const std::string& message) {
    if (level < s_minLevel) return;
    
    std::string timestamp = getTimestamp();
    std::string levelStr = levelToString(level);
    std::string logLine = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    if (s_consoleOutput) {
        std::cout << logLine << std::endl;
    }
    
    if (s_logFile.is_open()) {
        s_logFile << logLine << std::endl;
        s_logFile.flush();
    }
    
    if (level == LogLevel::Fatal) {
        std::abort();
    }
}

std::string Log::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Log::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:   return "TRACE";
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

} // namespace Omega
