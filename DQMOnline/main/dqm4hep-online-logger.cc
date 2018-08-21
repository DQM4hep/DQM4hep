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
#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/OnlineRoutes.h"
#include "dqm4hep/RemoteLogger.h"
#include "dqm4hep/Client.h"
#include "dqm4hep/DQM4hepConfig.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <ctime>
#include <atomic>
#include <signal.h>

using namespace std;
using namespace dqm4hep::online;
using namespace dqm4hep::net;
using namespace dqm4hep::core;

//-------------------------------------------------------------------------------------------------

std::atomic<bool> stopFlag(false);

// key interrupt signal handling
void int_key_signal_handler(int) {
  std::cout << std::endl;
  dqm_info( "Caught CTRL+C. Exiting..." );
  stopFlag = true;
}

//-------------------------------------------------------------------------------------------------

/**
 *  @brief Return the current time as std::string with format HH:MM:SS
 */
std::string currentTimeToString() {
  std::time_t t = std::time(nullptr);
  std::string currentTime;
  currentTime.resize(9);
  std::strftime(const_cast<char*>(currentTime.c_str()), 9, "%T", std::localtime(&t));
  return currentTime;
}

//-------------------------------------------------------------------------------------------------

/**
 *  @brief  LogPrint class
 */
class LogPrinter {
public:
  /**
   *  @brief  Constructor
   */
  LogPrinter(Logger::Level level, const StringVector &loggers, const StringVector &hosts) :
    m_logLevel(level),
    m_selectedLoggers(loggers),
    m_selectedHosts(hosts) {
  }
  
  /**
   *  @brief  Print a log message from received json buffer
   *  
   *  @param  buffer the json buffer containing the log message
   */
  void printLog(const Buffer &buffer) {
    // read message from buffer
    json logMessage;
    try {
      logMessage = json::parse(buffer.begin(), buffer.end());
    }
    catch(json::parse_error& e) {
      dqm_warning( "LogPrinter::printLog: received unexpected log message (json parse error)" );
      return;
    }
    
    // extract metadata
    std::string loggerName = logMessage.value<std::string>("logger", "");
    Logger::Level logLevel = static_cast<Logger::Level>(logMessage.value<int>("level", 0));
    std::string logLevelStr = Logger::logLevelToString(logLevel);
    std::string message = logMessage.value<std::string>("message", "");
    std::string host = logMessage.value<std::string>("host", "");
    int pid = logMessage.value<int>("pid", 0);
    
    // apply filters: logger and host names
    if(not m_selectedLoggers.empty()) {
      auto findIter = std::find(m_selectedLoggers.begin(), m_selectedLoggers.end(), loggerName);
      if(m_selectedLoggers.end() == findIter) {
        return;
      }
    }
    
    if(not m_selectedHosts.empty()) {
      auto findIter = std::find(m_selectedHosts.begin(), m_selectedHosts.end(), host);
      if(m_selectedHosts.end() == findIter) {
        return;
      }
    }
    
    // get the logger or create a new one
    Logger::LoggerPtr logger = Logger::logger( loggerName );
    if(not logger) {
      logger = Logger::createLogger(loggerName, {Logger::coloredConsole()});
      logger->set_level(m_logLevel);
      logger->set_pattern("%v");
    }
    
    // log message
    dqm_logger_log(logger, logLevel, 
      "[{0} on {1} ({2})] {3} - {4}: {5}", 
      loggerName, 
      host, 
      pid,
      currentTimeToString(),
      logLevelStr, 
      message);
  }
  
private:
  Logger::Level        m_logLevel;
  StringVector         m_selectedLoggers;
  StringVector         m_selectedHosts;
};

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4hep_VERSION_STR);

  StringVector verbosities(Logger::logLevels());
  TCLAP::ValuesConstraint<std::string> verbosityConstraint(verbosities);
  TCLAP::ValueArg<std::string> verbosityArg(
      "v"
      , "verbosity"
      , "The logging verbosity"
      , false
      , "info"
      , &verbosityConstraint);
  pCommandLine->add(verbosityArg);
  
  TCLAP::MultiArg<std::string> selectLoggersArg(
      "l"
      , "select-loggers"
      , "Print only log message for the specified loggers"
      , false
      , "string");
  pCommandLine->add(selectLoggersArg);
  
  TCLAP::MultiArg<std::string> selectHostsArg(
      "k"
      , "select-hosts"
      , "Print only log message for the specified hosts"
      , false
      , "string");
  pCommandLine->add(selectHostsArg);
  
  // parse command line
  pCommandLine->parse(argc, argv);
  
  std::string verbosity(verbosityArg.getValue());
  Logger::Level level = Logger::logLevelFromString(verbosity);
  
  const std::string localVerbosity("info");
  const std::string loggerName("online-logger:" + typeToString(dqm4hep::core::pid()));
  Logger::createLogger(loggerName, {Logger::coloredConsole()});
  Logger::setMainLogger(loggerName);
  Logger::setLogLevel(Logger::logLevelFromString(localVerbosity));

  // install signal handlers
  signal(SIGINT,  int_key_signal_handler);
  
  try {
    Client client;
    LogPrinter printer(level, selectLoggersArg.getValue(), selectHostsArg.getValue());
    client.subscribe(OnlineRoutes::OnlineManager::logs(), &printer, &LogPrinter::printLog);
    
    while(not stopFlag.load())
      dqm4hep::core::sleep(std::chrono::seconds(1));
  }
  catch(StatusCodeException &exception) {
    dqm_error( "Caught exception while running: {0}", exception.toString() );
    return exception.getStatusCode();
  }

  return 0;
}
