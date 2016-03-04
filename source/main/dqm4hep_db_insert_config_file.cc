  /// \file dqm4hep_db_insert_config_file.cc
/*
 *
 * dqm4hep_db_insert_config_file.cc main source file template automatically generated
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
#include "dqm4hep/DQMDBInterface.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>

using namespace std;
using namespace dqm4hep;

int main(int argc, char* argv[])
{
	std::string cmdLineFooter = "Please report bug to <rete@ipnl.in2p3.fr>";
	TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQMCore_VERSION_STR);
	std::string log4cxx_file = std::string(DQMCore_DIR) + "/conf/defaultLoggerConfig.xml";

	TCLAP::ValueArg<std::string> userArg(
				  "u"
				 , "user"
				 , "The MySQL database user name"
				 , false
				 , "DQM4HEP"
				 , "string");
	pCommandLine->add(userArg);

	TCLAP::ValueArg<std::string> hostArg(
				  "k"
				 , "host"
				 , "The MySQL database host name"
				 , false
				 , "localhost"
				 , "string");
	pCommandLine->add(hostArg);

	TCLAP::ValueArg<std::string> passwordArg(
				  "p"
				 , "password"
				 , "The MySQL database password"
				 , false
				 , "DQM4HEP"
				 , "string");
	pCommandLine->add(passwordArg);

	TCLAP::ValueArg<std::string> fileArg(
				  "f"
				 , "file"
				 , "The config file to store in database"
				 , true
				 , ""
				 , "string");
	pCommandLine->add(fileArg);

	TCLAP::ValueArg<std::string> configFileNameArg(
				  "c"
				 , "config-file-name"
				 , "The MySQL primary key for the config file to store in database"
				 , false
				 , ""
				 , "string");
	pCommandLine->add(configFileNameArg);

	TCLAP::ValueArg<std::string> fileDescriptionArg(
				  "d"
				 , "description"
				 , "The config file to insert in database"
				 , false
				 , ""
				 , "string");
	pCommandLine->add(fileDescriptionArg);

	TCLAP::SwitchArg forceReplaceArg(
				  "r"
				 , "force-replace"
				 , "Whether to force replace the config file entry in database"
				 , false);
	pCommandLine->add(forceReplaceArg);

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

	// parse command line
	pCommandLine->parse(argc, argv);

	log4cxx::xml::DOMConfigurator::configure(log4cxx_file);

	if( verbosityArg.isSet() )
		dqmMainLogger->setLevel( log4cxx::Level::toLevel( verbosityArg.getValue() ) );

	DQMDBInterface *pDBInterface = NULL;

	try
	{
		pDBInterface = new DQMDBInterface(hostArg.getValue(), userArg.getValue(), passwordArg.getValue(), "DQM4HEP");
	}
	catch(const StatusCodeException &exception)
	{
		if(NULL != pDBInterface)
			delete pDBInterface;

		return -1;
	}

	try
	{
		// construct the config file name primary key to insert in data base
		std::string configFilePrimaryKey;

		if(configFileNameArg.isSet())
		{
			configFilePrimaryKey = configFileNameArg.getValue();
		}
		else
		{
			size_t slashPos = fileArg.getValue().rfind("/");

			if(slashPos != std::string::npos)
				configFilePrimaryKey = fileArg.getValue().substr(slashPos+1);
			else
				configFilePrimaryKey = fileArg.getValue();
		}

		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pDBInterface->insertConfigFile(fileArg.getValue(), configFilePrimaryKey,
				fileDescriptionArg.getValue(), forceReplaceArg.getValue()));
	}
	catch(const StatusCodeException &exception)
	{
		if(NULL != pDBInterface)
			delete pDBInterface;

		return exception.getStatusCode();
	}

	std::cout << "OK" << std::endl;
	delete pDBInterface;

	return 0;
}
