/// \file dqm4hep-dump-event.cc
/*
 *
 * dqm4hep-dump-event.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2014
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

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


