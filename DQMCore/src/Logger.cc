//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/Logger.h>

// -- spdlog headers
#include <spdlog/logger.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace dqm4hep {

  namespace core {

    std::shared_ptr<spdlog::logger> Logger::m_mainLogger = Logger::createLogger("main", {Logger::coloredConsole()});

    //-------------------------------------------------------------------------------------------------

    std::shared_ptr<spdlog::logger> Logger::createLogger(const std::string &name,
                                                         std::initializer_list<Logger::AppenderPtr> appenders) {
      std::shared_ptr<spdlog::logger> logger_ptr(spdlog::get(name));

      if (nullptr != logger_ptr)
        return logger_ptr;

      logger_ptr = std::make_shared<spdlog::logger>(name, begin(appenders), end(appenders));
      spdlog::register_logger(logger_ptr);

      return logger_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    std::shared_ptr<spdlog::logger> Logger::createLogger(const std::string &name,
                                                         const std::vector<Logger::AppenderPtr> &appenders) {
      std::shared_ptr<spdlog::logger> logger_ptr(spdlog::get(name));

      if (nullptr != logger_ptr)
        return logger_ptr;

      logger_ptr = std::make_shared<spdlog::logger>("main", begin(appenders), end(appenders));
      spdlog::register_logger(logger_ptr);

      return logger_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    Logger::LoggerPtr Logger::mainLogger() {
      return m_mainLogger;
    }

    //-------------------------------------------------------------------------------------------------

    Logger::LoggerPtr Logger::setMainLogger(const std::string &logger) {
      LoggerPtr logger_ptr(Logger::logger(logger));

      if (nullptr != logger_ptr)
        m_mainLogger = logger_ptr;

      return m_mainLogger;
    }

    //-------------------------------------------------------------------------------------------------

    Logger::LoggerPtr Logger::logger(const std::string &name) {
      return spdlog::get(name);
    }

    //-------------------------------------------------------------------------------------------------

    void Logger::setLogLevel(Level level) {
      m_mainLogger->set_level(level);
    }

    //-------------------------------------------------------------------------------------------------

    std::string Logger::logLevelToString(Logger::Level level) {
      return spdlog::level::to_str(level);
    }

    //-------------------------------------------------------------------------------------------------

    Logger::Level Logger::logLevelFromString(const std::string &level) {
      if (level == "trace")
        return spdlog::level::trace;
      if (level == "debug")
        return spdlog::level::debug;
      if (level == "info")
        return spdlog::level::info;
      if (level == "warning")
        return spdlog::level::warn;
      if (level == "error")
        return spdlog::level::err;
      if (level == "critical")
        return spdlog::level::critical;
      if (level == "off")
        return spdlog::level::off;

      throw StatusCodeException(STATUS_CODE_NOT_FOUND);
    }

    //-------------------------------------------------------------------------------------------------

    StringVector Logger::logLevels() {
      return StringVector({logLevelToString(spdlog::level::trace), logLevelToString(spdlog::level::debug),
                           logLevelToString(spdlog::level::info), logLevelToString(spdlog::level::warn),
                           logLevelToString(spdlog::level::err), logLevelToString(spdlog::level::critical),
                           logLevelToString(spdlog::level::off)});
    }

    //-------------------------------------------------------------------------------------------------

    void Logger::setLoggerPattern(const std::string &logger, const std::string &pattern) {
      LoggerPtr logger_ptr = Logger::logger(logger);

      if (logger_ptr)
        logger_ptr->set_pattern(pattern);
    }

    //-------------------------------------------------------------------------------------------------

    Logger::AppenderPtr Logger::simpleFile(const std::string &fileName, bool truncated) {
      return std::make_shared<spdlog::sinks::simple_file_sink_mt>(fileName, truncated);
    }

    //-------------------------------------------------------------------------------------------------

    Logger::AppenderPtr Logger::rotatingFile(const std::string &baseFileName, size_t maxSize, size_t maxNFiles) {
      return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(baseFileName, maxSize, maxNFiles);
    }

    //-------------------------------------------------------------------------------------------------

    Logger::AppenderPtr Logger::dailyFile(const std::string &baseFileName, int rotationHour, int rotationMinute) {
      return std::make_shared<spdlog::sinks::daily_file_sink_mt>(baseFileName, rotationHour, rotationMinute);
    }

    //-------------------------------------------------------------------------------------------------

    Logger::AppenderPtr Logger::console() {
      return std::make_shared<spdlog::sinks::stdout_sink_mt>();
    }

    //-------------------------------------------------------------------------------------------------

    Logger::AppenderPtr Logger::coloredConsole() {
      return std::make_shared<spdlog::sinks::ansicolor_sink>(Logger::console());
    }
  }
}
