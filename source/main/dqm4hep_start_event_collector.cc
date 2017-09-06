/// \file dqm4hep_start_event_collector.cc
/*
 *
 * dqm4hep_start_event_collector.cc main source file template automatically generated
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
#include "DQMCoreConfig.h"
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/PluginManager.h"
#include "dqm4hep/EventStreamer.h"
#include "dqm4hep/EventCollectorApplication.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

EventCollectorApplication *pApplication = NULL;

// simple function to exit the program
void exit_application(int returnCode)
{
  LOG4CXX_WARN( dqmMainLogger , "Exiting event collector application !" );

  if(NULL != pApplication)
    pApplication->exit( returnCode );
  else
    exit(0);
}

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int signal)
{
  if(NULL == pApplication)
    exit(0);

  LOG4CXX_WARN( dqmMainLogger , "*** SIGN INT ***" );
  LOG4CXX_WARN( dqmMainLogger , "Caught signal " << signal << ". Stopping the application." );
  exit_application( static_cast<int>(STATUS_CODE_SUCCESS) );
}

//-------------------------------------------------------------------------------------------------

// segmentation violation signal handling
void seg_viol_signal_handling(int signal)
{
  if(NULL == pApplication)
    exit(1);

  LOG4CXX_WARN( dqmMainLogger , "*** SIGN VIOL ***" );
  LOG4CXX_WARN( dqmMainLogger , "Caught signal " << signal << ". Stopping the application." );
  exit_application( static_cast<int>(STATUS_CODE_INVALID_PTR) );
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  DQM4HEP::screenSplash();

  std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4HEP_VERSION_STR);
  std::string log4cxx_file = std::string(DQMCore_DIR) + "/conf/defaultLoggerConfig.xml";

  TCLAP::ValueArg<std::string> collectorNameArg(
      "c"
      , "collector-name"
      , "The event collector name to create"
      , true
      , ""
      , "string");
  pCommandLine->add(collectorNameArg);

  TCLAP::ValueArg<std::string> loggerConfigArg(
      "l"
      , "logger-config"
      , "The xml logger file to configure log4cxx"
      , false
      , log4cxx_file
      , "string");
  pCommandLine->add(loggerConfigArg);

  std::vector<std::string> allowedLevels;
  allowedLevels.push_back("INFO");
  allowedLevels.push_back("WARN");
  allowedLevels.push_back("DEBUG");
  allowedLevels.push_back("TRACE");
  allowedLevels.push_back("ERROR");
  allowedLevels.push_back("FATAL");
  allowedLevels.push_back("OFF");
  allowedLevels.push_back("ALL");
  TCLAP::ValuesConstraint<std::string> allowedLevelsContraint( allowedLevels );

  TCLAP::ValueArg<std::string> verbosityArg(
      "v"
      , "verbosity"
      , "The verbosity level used for this application"
      , false
      , "INFO"
      , &allowedLevelsContraint);
  pCommandLine->add(verbosityArg);

  TCLAP::ValueArg<std::string> streamerArg(
      "s"
      , "streamer"
      , "The event streamer used for this application"
      , false
      , "LCIO"  // an implementation provided by dqm4hep if LCIO is available
      , "string");
  pCommandLine->add(streamerArg);

  // parse command line
  std::cout << "dqm4hep_start_event_collector: Parsing command line ..." << std::endl;
  pCommandLine->parse(argc, argv);

  log4cxx::xml::DOMConfigurator::configure(log4cxx_file);

  if( verbosityArg.isSet() )
    dqmMainLogger->setLevel( log4cxx::Level::toLevel( verbosityArg.getValue() ) );

  // install signal handlers
  LOG4CXX_INFO( dqmMainLogger , "Installing signal handlers ..." );
  signal(SIGINT,  int_key_signal_handler);
  signal(SIGSEGV, seg_viol_signal_handling);

  LOG4CXX_INFO( dqmMainLogger , "Creating event collector application ..." );

  try
  {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PluginManager::instance()->loadLibraries());

    pApplication = new EventCollectorApplication();
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pApplication->setCollectorName(collectorNameArg.getValue()));

    // configure streamer
    if(streamerArg.isSet())
    {
      EventStreamer *pEventStreamer = PluginManager::instance()->createPluginClass<EventStreamer>(streamerArg.getValue());

      if(!pEventStreamer)
        throw StatusCodeException(STATUS_CODE_FAILURE);

      pApplication->setEventStreamer(pEventStreamer);
    }
  }
  catch(StatusCodeException &exception)
  {
    LOG4CXX_FATAL( dqmMainLogger , "StatusCodeException caught : " << exception.toString() );
    exit_application( exception.getStatusCode() );
  }

  LOG4CXX_INFO( dqmMainLogger , "Creating event collector application ... OK" );
  LOG4CXX_INFO( dqmMainLogger , "Running event collector application ... OK" );

  try
  {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pApplication->run());
  }
  catch(StatusCodeException &exception)
  {
    LOG4CXX_FATAL( dqmMainLogger , "StatusCodeException caught : " << exception.toString() );
    exit_application( exception.getStatusCode() );
  }

  delete pCommandLine;
  exit_application( static_cast<int>(STATUS_CODE_SUCCESS) );
}
