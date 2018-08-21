//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_LOGGING_H
#define DQM4HEP_LOGGING_H

#include <dqm4hep/Logger.h>

#define dqm_logger_log(logger, level, ...) logger->log(level, __VA_ARGS__)
#define dqm_logger_trace(logger, ...) logger->trace(__VA_ARGS__)
#define dqm_logger_debug(logger, ...) logger->debug(__VA_ARGS__)
#define dqm_logger_warning(logger, ...) logger->warn(__VA_ARGS__)
#define dqm_logger_error(logger, ...) logger->error(__VA_ARGS__)
#define dqm_logger_critical(logger, ...) logger->critical(__VA_ARGS__)
#define dqm_logger_info(logger, ...) logger->info(__VA_ARGS__)

#define dqm_log(level, ...) dqm4hep::core::Logger::mainLogger()->log(level, __VA_ARGS__)
#define dqm_trace(...) dqm4hep::core::Logger::mainLogger()->trace(__VA_ARGS__)
#define dqm_debug(...) dqm4hep::core::Logger::mainLogger()->debug(__VA_ARGS__)
#define dqm_warning(...) dqm4hep::core::Logger::mainLogger()->warn(__VA_ARGS__)
#define dqm_error(...) dqm4hep::core::Logger::mainLogger()->error(__VA_ARGS__)
#define dqm_critical(...) dqm4hep::core::Logger::mainLogger()->critical(__VA_ARGS__)
#define dqm_info(...) dqm4hep::core::Logger::mainLogger()->info(__VA_ARGS__)

#endif //  DQM4HEP_LOGGING_H
