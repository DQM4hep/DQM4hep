//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_LOGGER_H
#define DQM4HEP_LOGGER_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>

// -- std headers
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

// -- spdlog headers
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace dqm4hep {

  namespace core {

    class Logger {
    public:
      typedef std::shared_ptr<spdlog::logger> LoggerPtr;
      typedef std::shared_ptr<spdlog::sinks::sink> AppenderPtr;
      typedef spdlog::level::level_enum Level;

    public:
      /**
       * Create a new logger with the target appenders
       *
       * @param  name      the logger name
       * @param  appenders the list of appenders
       * @return           a logger pointer (smart ptr)
       */
      static LoggerPtr createLogger(const std::string &name, std::initializer_list<AppenderPtr> appenders);

      /**
       * Create a new logger with the target appenders
       *
       * @param  name      the logger name
       * @param  appenders the list of appenders
       * @return           a logger pointer (smart ptr)
       */
      static LoggerPtr createLogger(const std::string &name, const std::vector<AppenderPtr> &appenders);

      /**
       * Create a simple file appender
       * @param  fileName  the file name
       * @param  truncated whether to truncate the file if needed
       * @return           an appender pointer (smart pointer)
       */
      static AppenderPtr simpleFile(const std::string &fileName, bool truncated = false);

      /**
       * Create a rotating file appender
       * @param  baseFileName the base file name
       * @param  maxSize      the maximum single file size (unit bytes)
       * @param  maxNFiles    the maximum number of files to rotate with
       * @return              an appender pointer (smart pointer)
       */
      static AppenderPtr rotatingFile(const std::string &baseFileName, size_t maxSize, size_t maxNFiles);

      /**
       * Create a daily file appender
       * @param  baseFileName   the base file name
       * @param  rotationHour   the hour for rotation
       * @param  rotationMinute the minute for rotation
       * @return                an appender pointer (smart pointer)
       */
      static AppenderPtr dailyFile(const std::string &baseFileName, int rotationHour, int rotationMinute);

      /**
       * Create a console appender
       * @return an appender pointer (smart pointer)
       */
      static AppenderPtr console();

      /**
       * Create a colored console appender
       * @return an appender pointer (smart pointer)
       */
      static AppenderPtr coloredConsole();

      /**
       * Create a remote appender
       * @return an appender pointer (smart pointer)
       */
      static AppenderPtr remote();

      /**
       * Get an existing logger
       * @param  name the logger name
       * @return      a logger pointer (smart pointer) or nullptr if doesn't exists
       */
      static LoggerPtr logger(const std::string &name);

      /**
       * Set the log level of the main logger
       * @param level the log level
       */
      static void setLogLevel(Level level);

      /**
       * Get the main logger
       * @return  a logger pointer (smart pointer)
       */
      static LoggerPtr mainLogger();

      /**
       * Change the main logger or the target one
       * @param  logger the new main logger name
       * @return        a pointer to the main logger
       */
      static LoggerPtr setMainLogger(const std::string &logger);

      /**
       * Convert log level to std::string
       * @param  level the log level
       * @return       log level in string format
       */
      static std::string logLevelToString(Level level);

      /**
       * Convert std::string to log level
       * @param  level the log level string
       * @return       the log level
       */
      static Level logLevelFromString(const std::string &level);

      /**
       * Get the log levels list as strings
       */
      static StringVector logLevels();

      /**
       * Set the logger log format pattern
       *
       * %v 	The actual text to log
       * %t 	Thread id
       * %P 	Process id
       * %n 	Logger's name
       * %l 	The log level of the message
       * %L 	Short log level of the message, i.e	"D", "I", etc
       * %a 	Abbreviated weekday name, i.e "Thu"
       * %A 	Full weekday name, i.e "Thursday"
       * %b 	Abbreviated month name, i.e "Aug"
       * %B 	Full month name, i.e "August"
       * %c 	Date and time representation, i.e "Thu Aug 23 15:35:46 2014"
       * %C 	Year in 2 digits, i.e "14"
       * %Y 	Year in 4 digits, i.e "2014"
       * %D   (or %x) 	Short MM/DD/YY date
       * %m 	Month 1-12
       * %d 	Day of month 1-31
       * %H 	Hours in 24 format 0-23
       * %I 	Hours in 12 format 1-12
       * %M 	Minutes 0-59
       * %S 	Seconds 0-59
       * %e 	Millisecond part of the current second 0-999 	"678"
       * %f 	Microsecond part of the current second 0-999999 	"056789"
       * %F 	Nanosecond part of the current second 0-999999999 	"256789123"
       * %p 	AM/PM 	"AM"
       * %r 	12 hour clock 	"02:55:02 pm"
       * %R 	24-hour HH:MM time, equivalent to %H:%M 	"23:55"
       * %T   (or %X) 	ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S 	"23:55:59"
       * %z 	ISO 8601 offset from UTC in timezone ([+/-]HH:MM) 	"+02:00"
       * %% 	The % sign 	"%"
       * %+ 	default format 	"[2014-31-10 23:46:59.678] [mylogger] [info] Some message"
       *
       * @param logger  the logger name
       * @param pattern the logging pattern
       */
      static void setLoggerPattern(const std::string &logger, const std::string &pattern);

    private:
      static LoggerPtr m_mainLogger; ///< The main logger instance
    };
  }
}

#endif //  DQM4HEP_LOGGER_H
