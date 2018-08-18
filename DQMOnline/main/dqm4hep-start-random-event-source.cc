/// \file dqm4hep_start_run_control_server.cc
/*
 *
 * dqm4hep_start_run_control_server.cc main source file template automatically generated
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
#include "dqm4hep/RemoteLogger.h"
#include "dqm4hep/EventSource.h"
#include "dqm4hep/GenericEvent.h"
#include "dqm4hep/DQM4hepConfig.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <atomic>
#include <signal.h>
#include <random>

using namespace std;
using namespace dqm4hep::online;
using namespace dqm4hep::core;

std::atomic_bool running(true);

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int)
{
  std::cout << std::endl;
  dqm_info( "Caught CTRL+C. Exiting..." );
  running = false;
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  dqm4hep::core::screenSplash();

  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4hep_VERSION_STR);

  TCLAP::ValueArg<std::string> sourceNameArg(
      "n"
      , "source-name"
      , "The event source name"
      , false
      , "RandomSource"
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
  
  TCLAP::MultiArg<std::string> collectorNameArg(
      "c"
      , "collector"
      , "List of collectors to send the event to"
      , true
      , "string");
  pCommandLine->add(collectorNameArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  // install signal handlers
  signal(SIGINT,  int_key_signal_handler);

  // set log level
  std::string verbosity(verbosityArg.getValue());
  Logger::createLogger("rand-src:" + sourceNameArg.getValue(), {Logger::coloredConsole(), RemoteLogger::make_shared()});
  Logger::setMainLogger("rand-src:" + sourceNameArg.getValue());
  Logger::setLogLevel(Logger::logLevelFromString(verbosity));

  //
  auto eventSource = EventSource::make_shared(sourceNameArg.getValue());
  const StringVector &collectors(collectorNameArg.getValue());
  
  for(auto collector : collectors)
    eventSource->addCollector(collector);

  eventSource->start();
  
  uint32_t eventNumber(0);
  uint32_t runNumber((rand() / float(RAND_MAX))*125632);
  std::random_device rd{};
  std::mt19937 gen{rd()};
  std::normal_distribution<> d{5,2};
  
  while(running)
  {
    EventPtr event = GenericEvent::make_shared();
    event->setTimeStamp(dqm4hep::core::now());
    event->setType(CUSTOM_EVENT);
    event->setSource(sourceNameArg.getValue());
    event->setRunNumber(runNumber);
    event->setStreamerName("GenericEventStreamer");
    event->setEventNumber(eventNumber);
    
    GenericEvent *generic = event->getEvent<GenericEvent>();
    std::vector<float> values;
    int nValues(std::round((rand()/float(RAND_MAX))*10) + 1);
    
    for(int i=0 ; i<nValues ; i++)
      values.push_back(d(gen));
    
    generic->setValues("data", values);
    
    eventSource->sendEvent(event);
    ++eventNumber;
    sleep(1);
  }

  return 0;
}
