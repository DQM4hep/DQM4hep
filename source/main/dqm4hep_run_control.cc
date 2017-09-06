/// \file dqm4hep_run_control.cc
/*
 *
 * dqm4hep_run_control.cc main source file template automatically generated
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
#include "dqm4hep/Logging.h"
#include "dqm4hep/Run.h"
#include "dqm4hep/DimRunControlClient.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4HEP_VERSION_STR);
  std::string log4cxx_file = std::string(DQMCore_DIR) + "/conf/defaultLoggerConfig.xml";

  TCLAP::ValueArg<std::string> runControlNameArg(
      "r"
      , "run-control-name"
      , "The run control name"
      , true
      , ""
      , "string");
  pCommandLine->add(runControlNameArg);

  TCLAP::ValueArg<std::string> passwordArg(
      "k"
      , "password"
      , "The run control password to execute command"
      , false
      , ""
      , "string");
  pCommandLine->add(passwordArg);

  TCLAP::ValueArg<int> startNewRunArg(
      "s"
      , "start-new-run"
      , "Start a new run"
      , true
      , 1
      , "int");

  TCLAP::SwitchArg endCurrentRunArg(
      "e"
      , "end-current-run"
      , "End the current run"
      , true);

  TCLAP::SwitchArg runStateArg(
      "c"
      , "run-state"
      , "Get the current state"
      , true);

  std::vector<TCLAP::Arg*>  xorlist;
  xorlist.push_back(&endCurrentRunArg);
  xorlist.push_back(&startNewRunArg);
  xorlist.push_back(&runStateArg);
  pCommandLine->xorAdd( xorlist );

  TCLAP::ValueArg<std::string> loggerConfigArg(
      "l"
      , "logger-file"
      , "The log4cxx logger xml file"
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

  TCLAP::MultiArg<std::string> runParametersArg(
      "p"
      , "parameter"
      , "A run parameter. To be used with --start-new-run option"
      , false
      , "");
  pCommandLine->add(runParametersArg);

  TCLAP::ValueArg<std::string> detectorNameArg(
      "d"
      , "detector-name"
      , "The detector name. To be used with --start-new-run option"
      , false
      , ""
      , "string");
  pCommandLine->add(detectorNameArg);

  TCLAP::ValueArg<std::string> descriptionArg(
      "t"
      , "description"
      , "The description text to join with a start of run. To be used with --start-new-run option"
      , false
      , ""
      , "string");
  pCommandLine->add(descriptionArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  log4cxx_file = loggerConfigArg.getValue();
  log4cxx::xml::DOMConfigurator::configure(log4cxx_file);

  if( verbosityArg.isSet() )
    dqmMainLogger->setLevel( log4cxx::Level::toLevel( verbosityArg.getValue() ) );

  std::string runControlName( runControlNameArg.getValue() );
  std::string password( passwordArg.getValue() );

  DimRunControlClient client;

  client.setRunControlName(runControlName);
  client.setPassword(password);

  if( startNewRunArg.isSet() )
  {
    Run *pRun = new Run();

    pRun->setRunNumber(startNewRunArg.getValue());
    pRun->setDetectorName(detectorNameArg.getValue());
    pRun->setDescription(descriptionArg.getValue());
    std::vector<std::string> parameters( runParametersArg.getValue() );

    for( auto iter = parameters.begin(), endIter = parameters.end() ; endIter != iter ; ++iter )
    {
      std::vector<std::string> keyValue;
      DQM4HEP::tokenize( *iter, keyValue, "=" );

      if( keyValue.size() != 2 )
      {
        LOG4CXX_WARN( dqmMainLogger, "Couldn't decode parameter '" << *iter << "' ! Skipping ..." );
        continue;
      }

      pRun->setParameter( keyValue[0] , keyValue[1] );
    }

    client.sendStartNewRun( pRun, password );
  }
  else if( endCurrentRunArg.isSet() )
  {
    client.sendEndCurrentRun( password );
  }
  else if( runStateArg.isSet() )
  {
    client.connectToService();

    if( client.isRunning() )
    {
      Run *pRun = client.getCurrentRun();

      LOG4CXX_INFO( dqmMainLogger ,"===== Run state ====" );
      LOG4CXX_INFO( dqmMainLogger , *pRun );
    }
    else
    {
      LOG4CXX_INFO( dqmMainLogger , "No run at this moment !" );
    }

    client.disconnectFromService();
  }

  return 0;
}
