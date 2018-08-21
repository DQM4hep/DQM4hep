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
#include "dqm4hep/PluginManager.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/OnlineRoutes.h"
#include "dqm4hep/EventCollectorClient.h"
#include "dqm4hep/DQM4hepConfig.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <atomic>
#include <signal.h>

using namespace std;
using namespace dqm4hep::net;
using namespace dqm4hep::online;
using namespace dqm4hep::core;

std::atomic_bool running(true);

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int /*signal*/)
{
  std::cout << std::endl;
  dqm_info( "Caught CTRL+C. Exiting..." );
  running = false;
}

//-------------------------------------------------------------------------------------------------

class EventPrinter {
public:
  void printEvent(EventPtr event);
};

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  dqm4hep::core::screenSplash();

  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4hep_VERSION_STR);

  TCLAP::ValueArg<std::string> collectorNameArg(
      "c"
      , "collector-name"
      , "The event collector name"
      , true
      , ""
      , "string");
  pCommandLine->add(collectorNameArg);
  
  TCLAP::ValueArg<std::string> sourceNameArg(
      "s"
      , "source-name"
      , "The event source name"
      , true
      , ""
      , "string");
  pCommandLine->add(sourceNameArg);

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

  // parse command line
  pCommandLine->parse(argc, argv);

  // install signal handlers
  signal(SIGINT,  int_key_signal_handler);

  // set log level
  std::string verbosity(verbosityArg.getValue());
  Logger::createLogger("dump-evt", {Logger::coloredConsole()});
  Logger::setMainLogger("dump-evt");
  Logger::setLogLevel(Logger::logLevelFromString(verbosity));
  
  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PluginManager::instance()->loadLibraries());
  }
  catch(StatusCodeException &e) {
    dqm_error( "Couldn't load plugins: {0}", e.toString() );
    return e.getStatusCode();
  }
  
  EventCollectorClient client(collectorNameArg.getValue());
  EventPrinter printer;
  client.onEventUpdate(sourceNameArg.getValue(), &printer, &EventPrinter::printEvent);
  client.startEventUpdates();
  
  while(running) {
    dqm4hep::core::sleep(std::chrono::milliseconds(100));
  }
  
  delete pCommandLine;

  return 0;
}

//-------------------------------------------------------------------------------------------------

void EventPrinter::printEvent(EventPtr event) {
  if(not event) {
    dqm_info( "==== Event is nullptr ! ====" );
    return;
  }
  std::string timeHMS;  
  timeToHMS(std::chrono::system_clock::to_time_t(event->getTimeStamp()), timeHMS);
  dqm_info(  "==== Event: run {0}, event {1} ====", event->getRunNumber(), event->getEventNumber() );
  dqm_debug( "====    Source:         {0}", event->getSource() );
  dqm_info(  "====    Time stamp:     {0}", timeHMS );
  dqm_info(  "====    Size:           {0} bytes", event->getEventSize() );
  dqm_info(  "" );
}


