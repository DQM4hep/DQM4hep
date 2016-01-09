  /// \file dqm4hep_start_monitor_element_collector.cc
/*
 *
 * dqm4hep_start_monitor_element_collector.cc main source file template automatically generated
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
#include "dqm4hep/DQMPluginManager.h"
#include "dqm4hep/DQMMonitorElementCollectorApplication.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep;

DQMMonitorElementCollectorApplication *pApplication = NULL;

// simple function to exit the program
void exit_application(int returnCode)
{
	streamlog_out(MESSAGE) << "Exiting application !" << std::endl;

	if(NULL != pApplication)
		pApplication->exit( returnCode );
	else
		exit(0);
}

// key interrupt signal handling
void int_key_signal_handler(int signal)
{
	if(NULL == pApplication)
		exit(0);

	streamlog_out(WARNING) << "*** SIGN INT ***" << std::endl;

	streamlog_out(MESSAGE) << "Caught signal " << signal << ". Stopping the application." << std::endl;
	exit_application( static_cast<int>(STATUS_CODE_SUCCESS) );
}

// segmentation violation signal handling
void seg_viol_signal_handling(int signal)
{
	if(NULL == pApplication)
		exit(1);

	streamlog_out(MESSAGE) << "*** SEG VIOL ***" << std::endl;
	streamlog_out(MESSAGE) << "Caught signal : " << signal << std::endl;
	exit_application( static_cast<int>(STATUS_CODE_INVALID_PTR) );
}


int main(int argc, char* argv[])
{
	DQM4HEP::screenSplash();

	std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
	TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4HEP_VERSION_STR);

	TCLAP::ValueArg<std::string> collectorNameArg(
				  "c"
				 , "collector-name"
				 , "The monitor element collector name to create"
				 , true
				 , ""
				 , "string");
	pCommandLine->add(collectorNameArg);

	TCLAP::SwitchArg loadLibrariesArg(
				  "l"
				 , "load-libraries"
				 , "Whether external libraries have to be loaded (DQM4HEP_PLUGIN_DLL env var)"
				 , false);
	pCommandLine->add(loadLibrariesArg);

	TCLAP::ValueArg<std::string> verbosityArg(
				  "v"
				 , "verbosity"
				 , "The verbosity used for this application"
				 , false
				 , "MESSAGE"
				 , "string");
	pCommandLine->add(verbosityArg);

	// parse command line
	std::cout << "dqm4hep_start_monitor_element_collector: Parsing command line ..." << std::endl;
	pCommandLine->parse(argc, argv);

	std::string collectorNameToUpper(collectorNameArg.getValue());
	std::string verbosity = verbosityArg.getValue();

	std::transform(collectorNameToUpper.begin(), collectorNameToUpper.end(), collectorNameToUpper.begin(), ::toupper);
	std::string applicationNameToUpper = "DQM4HEP MONITOR ELEMENT COLLECTOR " + collectorNameToUpper;
	streamlog_init( applicationNameToUpper , verbosity );

	// install signal handlers
	streamlog_out(MESSAGE) << "Installing signal handlers ... " << std::endl;
	signal(SIGINT,  int_key_signal_handler);
	signal(SIGSEGV, seg_viol_signal_handling);

	streamlog_out(MESSAGE) << "Creating application ... " << std::endl;

	try
	{
		if(loadLibrariesArg.getValue())
		{
			THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, DQMPluginManager::instance()->loadLibraries());
		}

		pApplication = new DQMMonitorElementCollectorApplication();
		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pApplication->setCollectorName(collectorNameArg.getValue()));
	}
	catch(StatusCodeException &exception)
	{
		streamlog_out(ERROR) << "StatusCodeException caught : " << exception.toString() << std::endl;
		exit_application( exception.getStatusCode() );
	}

	streamlog_out(MESSAGE) << "Creating application ... OK" << std::endl;


	streamlog_out(MESSAGE) << "Running application ... " << std::endl;
	try
	{
		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pApplication->run());
	}
	catch(StatusCodeException &exception)
	{
		streamlog_out(ERROR) << "StatusCodeException caught : " << exception.toString() << std::endl;
		exit_application( exception.getStatusCode() );
	}

	delete pCommandLine;
	exit_application( static_cast<int>(STATUS_CODE_SUCCESS) );
}
