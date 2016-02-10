  /// \file dqm4hep_start_lcio_file_service.cc
/*
 *
 * dqm4hep_start_lcio_file_service.cc main source file template automatically generated
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

#include <iostream>

// -- dqm4hep
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMLogging.h"
#include "dqm4hep/DQMPluginManager.h"
#include "dqm4hep/DQMDimEventClient.h"
#include "dqm4hep/lcio/DQMLcioReaderListener.h"
#include "dqm4hep/lcio/DQMLCEventStreamer.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- lcio headers
#include "IO/LCReader.h"
#include "IOIMPL/LCFactory.h"

// -- dim headers
#include "dis.hxx"

using namespace std;
using namespace dqm4hep;

int main(int argc, char* argv[])
{
	DQM4HEP::screenSplash();

	std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
	TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4HEP_VERSION_STR);

	TCLAP::ValueArg<unsigned int> sleepTimeArg(
				  "t"
				 , "sleep-time"
				 , "The sleep time between each event (in usec)"
				 , false
				 , 100000
				 , "unsigned int");
	pCommandLine->add(sleepTimeArg);

	TCLAP::ValueArg<std::string> lcioFileNamesArg(
				  "f"
				 , "lcio-files"
				 , "The list of lcio files to process (separated by a ':' character)"
				 , true
				 , ""
				 , "string");
	pCommandLine->add(lcioFileNamesArg);

	TCLAP::ValueArg<std::string> eventCollectorNameArg(
				  "c"
				 , "collector-name"
				 , "The event collector name in which the event will be published"
				 , true
				 , ""
				 , "string");
	pCommandLine->add(eventCollectorNameArg);

	TCLAP::SwitchArg rewindArg(
				  "r"
				 , "rewind"
				 , "Whether the lcio file(s) should be rewinded"
				 , false);
	pCommandLine->add(rewindArg);

	TCLAP::ValueArg<std::string> verbosityArg(
				  "v"
				 , "verbosity"
				 , "The verbosity used for this application"
				 , false
				 , "WARNING"
				 , "string");
	pCommandLine->add(verbosityArg);

	TCLAP::SwitchArg simulateSpillArg(
				  "s"
				 , "simulate-spill"
				 , "Whether a spill structure has to be simulated using getTimeStamp() of LCEvents"
				 , false);
	pCommandLine->add(simulateSpillArg);

	// parse command line
	pCommandLine->parse(argc, argv);

	std::string verbosity = verbosityArg.getValue();
	streamlog_init( "DQM4HEP LCIO FILE SERVICE" , verbosity );

	bool shouldRewind = rewindArg.getValue();
	std::vector<std::string> lcioInputFiles;
	DQM4HEP::tokenize(lcioFileNamesArg.getValue(), lcioInputFiles, ":");

	// file reader
	IO::LCReader *pLCReader = IOIMPL::LCFactory::getInstance()->createLCReader(1);

	// event collector client
	DQMEventClient *pEventClient = new DQMDimEventClient();

	DQMLCEventStreamer *pEventStreamer = DQMPluginManager::instance()->createPluginClass<DQMLCEventStreamer>("LCIOStreamer");

	if(!pEventStreamer)
	{
		streamlog_out(ERROR) << "Couldn't get the lcio streamer" << std::endl;
		streamlog_out(ERROR) << "Plugin registered ? " << DQMPluginManager::instance()->isPluginRegistered("LCIOStreamer") << std::endl;

		return 1;
	}

	pEventClient->setEventStreamer(pEventStreamer);
	pEventClient->setCollectorName(eventCollectorNameArg.getValue());

	// lcio file listener
	DQMLcioReaderListener *pListener = new DQMLcioReaderListener(pLCReader);
	pListener->setSimulateSpill(simulateSpillArg.getValue());
	pListener->setEventClient(pEventClient);
	pListener->setSleepTime(sleepTimeArg.getValue());

	while(1)
	{
		try
		{
		   pLCReader->open(lcioInputFiles);
		   pLCReader->readStream();
		   pLCReader->close();
		}
		catch(StatusCodeException &exception)
		{
			streamlog_out(ERROR) << "*********************************************************" << std::endl;
			streamlog_out(ERROR) << "*                                                       *" << std::endl;
			streamlog_out(ERROR) << "*   StatusCodeException caught : " << exception.toString() << std::endl;
			streamlog_out(ERROR) << "*     Stop processing file(s) through the service !     *" << std::endl;
			streamlog_out(ERROR) << "*                                                       *" << std::endl;
			streamlog_out(ERROR) << "*********************************************************" << std::endl;

			pLCReader->close();
			shouldRewind = false;
			break;
		}
		catch(std::exception & exception)
		{
			streamlog_out(ERROR) << "*********************************************************" << std::endl;
			streamlog_out(ERROR) << "*                                                       *" << std::endl;
			streamlog_out(ERROR) << "*     std::exception caught : " << exception.what() << std::endl;
			streamlog_out(ERROR) << "*     Stop processing file(s) through the service !     *" << std::endl;
			streamlog_out(ERROR) << "*                                                       *" << std::endl;
			streamlog_out(ERROR) << "*********************************************************" << std::endl;

			pLCReader->close();
			shouldRewind = false;
			break;
		}

		if(shouldRewind)
		{
			streamlog_out(MESSAGE) << "Rewinding lcio files" << std::endl;
		}
		else
		{
			streamlog_out(MESSAGE) << "Exiting lcio file service ..." << std::endl;
			break;
		}

	}

	delete pEventClient;
	delete pListener;
	delete pLCReader;
	delete pCommandLine;

	return 0;
}
