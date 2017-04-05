/// \file dqm4hep-start-log-collector.cc
/*
 *
 * dqm4hep-start-log-collector.cc main source file template automatically generated
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
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/LogCollector.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

LogCollector *pLogCollector = nullptr;

// simple function to exit the program
void exit_application(int returnCode)
{
  if(nullptr != pLogCollector)
    pLogCollector->stop();

  if(returnCode == 0)
    dqm_info("Exiting application with code {}", returnCode);
  else
    dqm_error("Exiting application with code {}", returnCode);

  exit(returnCode);
}

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int signal)
{
  exit_application(0);
}

//-------------------------------------------------------------------------------------------------

// segmentation violation signal handling
void seg_viol_signal_handling(int signal)
{
  exit_application(1);
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  DQM4HEP::screenSplash();

  std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4HEP_VERSION_STR);

  TCLAP::ValueArg<size_t> maxSizeArg(
      "s"
      , "logs-size"
      , "The maximum log stack size"
      , false
      , 1000
      , "size_t");
  pCommandLine->add(maxSizeArg);

  TCLAP::ValueArg<unsigned int> cleaningPeriodArg(
      "t"
      , "cleaning-period"
      , "The period after which the logs are cleaned up (unit minutes)"
      , false
      , 60
      , "unsigned int");
  pCommandLine->add(cleaningPeriodArg);

  std::vector<std::string> verbosityConstraints({"trace", "debug", "info",  "warning", "error", "critical", "off"});
  TCLAP::ValuesConstraint<std::string> allowedLevelsContraint(verbosityConstraints);
  TCLAP::ValueArg<std::string> verbosityArg(
      "v"
      , "verbosity"
      , "The verbosity level used for this application"
      , false
      , "info"
      , &allowedLevelsContraint);
  pCommandLine->add(verbosityArg);

  auto iter = std::find(verbosityConstraints.begin(), verbosityConstraints.end(), verbosityArg.getValue());
  Logger::Level level(static_cast<Logger::Level>(std::distance(verbosityConstraints.begin(), iter)));

  // parse command line
  std::cout << "dqm4hep-start-log-collector : Parsing command line ..." << std::endl;
  pCommandLine->parse(argc, argv);

  Logger::createLogger("log-collector", {Logger::coloredConsole()});
  Logger::setMainLogger("log-collector");
  Logger::setLogLevel(level);

  // install signal handlers
  signal(SIGINT,  int_key_signal_handler);
  signal(SIGSEGV, seg_viol_signal_handling);

  try
  {
    pLogCollector = new LogCollector();
    pLogCollector->setMaxSize(maxSizeArg.getValue());
    pLogCollector->setCleaningPeriod(cleaningPeriodArg.getValue());
    pLogCollector->run();
  }
  catch(StatusCodeException &exception)
  {
    dqm_error("Caught exception : {}", exception.getStatusCode());
    exit_application(11);
  }

  // delete pCommandLine;
  exit_application(0);
}
