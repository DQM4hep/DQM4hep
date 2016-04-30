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
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMLogging.h"
#include "dqm4hep/DQMCoreTool.h"
#include "dqm4hep/DQMRunControlService.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <signal.h>

#ifdef DQM4HEP_USE_MONGOOSE
#include <mongoose/Server.h>
#endif

using namespace std;
using namespace dqm4hep;

bool process = true;

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int signal)
{
	LOG4CXX_WARN( dqmMainLogger , "*** SIGN INT ***" );
	LOG4CXX_WARN( dqmMainLogger , "Caught signal " << signal << ". Stopping the application." );

	process = false;
}

//-------------------------------------------------------------------------------------------------

// segmentation violation signal handling
void seg_viol_signal_handling(int signal)
{
	LOG4CXX_WARN( dqmMainLogger , "*** SIGN VIOL ***" );
	LOG4CXX_WARN( dqmMainLogger , "Caught signal " << signal << ". Stopping the application." );
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	DQM4HEP::screenSplash();

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
				 , "The run control password to execute command from interfaces"
				 , false
				 , ""
				 , "string");
	pCommandLine->add(passwordArg);

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


#ifdef DQM4HEP_USE_MONGOOSE
	TCLAP::ValueArg<unsigned int> httpPortArg(
				  "p"
				 , "http-port"
				 , "The http port to listen on (with mongoose)"
				 , false
				 , 8082
				 , "unsigned int");
	pCommandLine->add(httpPortArg);
#endif

	// parse command line
	pCommandLine->parse(argc, argv);

	log4cxx_file = loggerConfigArg.getValue();
	log4cxx::xml::DOMConfigurator::configure(log4cxx_file);

	if( verbosityArg.isSet() )
		dqmMainLogger->setLevel( log4cxx::Level::toLevel( verbosityArg.getValue() ) );

	// install signal handlers
	LOG4CXX_INFO( dqmMainLogger , "Installing signal handlers ..." );
	signal(SIGINT,  int_key_signal_handler);
//	signal(SIGSEGV, seg_viol_signal_handling);


	std::string runControlName( runControlNameArg.getValue() );
	std::string password( passwordArg.getValue() );

	DQMRunControlService runControlServer;

	runControlServer.setRunControlName(runControlName);
	runControlServer.setPassword(password);

#ifdef DQM4HEP_USE_MONGOOSE
	Server server( httpPortArg.getValue() );
	server.registerController( &runControlServer );
	server.start();

	runControlServer.dumpRoutes();
#endif

	try
	{
		THROW_RESULT_IF( STATUS_CODE_SUCCESS , !=, runControlServer.start() );

		DimServer::start(  ("DQM4HEP/RunControl/" + runControlName).c_str() );

		while( process )
			DQMCoreTool::sleep(std::chrono::seconds(1));
	}
	catch(StatusCodeException &exception)
	{
		LOG4CXX_ERROR( dqmMainLogger , "Couldn't start run control server : " << exception.toString() );

#ifdef DQM4HEP_USE_MONGOOSE
		server.stop();
#endif

		return 1;
	}

#ifdef DQM4HEP_USE_MONGOOSE
		server.stop();
#endif

	return 0;
}
