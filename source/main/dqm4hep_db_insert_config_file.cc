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

	TCLAP::ValueArg<std::string> userArg(
				  "u"
				 , "user"
				 , "The MySQL database user name"
				 , true
				 , ""
				 , "string");
	pCommandLine->add(userArg);

	TCLAP::ValueArg<std::string> hostArg(
				  "k"
				 , "host"
				 , "The MySQL database host name"
				 , true
				 , ""
				 , "string");
	pCommandLine->add(hostArg);

	TCLAP::ValueArg<std::string> passwordArg(
				  "p"
				 , "password"
				 , "The MySQL database password"
				 , true
				 , ""
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

	TCLAP::ValueArg<std::string> verbosityArg(
				  "v"
				 , "verbosity"
				 , "The verbosity used for this application"
				 , false
				 , "MESSAGE"
				 , "string");
	pCommandLine->add(verbosityArg);

	// parse command line
	pCommandLine->parse(argc, argv);

	std::string verbosity = verbosityArg.getValue();
	streamlog_init( "DB INSERT" , verbosity );

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

		streamlog_out(DEBUG) << "Config file key : " << configFilePrimaryKey << std::endl;
//
//		std::ifstream ifile;
//		ifile.open(fileArg.getValue().c_str(), std::ios::in);
//
//		if(!ifile.is_open())
//		{
//			streamlog_out(ERROR) << "Couln't open file '" << fileArg.getValue() << "' !" << std::endl;
//			throw StatusCodeException(STATUS_CODE_FAILURE);
//		}
//
//		std::string fileContents;
//
//		while(!ifile.eof())
//		{
//			std::string str;
//			getline(ifile, str);
//			fileContents += str + "\n";
//		}
//
//		streamlog_out(DEBUG) << "File contents : " << std::endl;
//		streamlog_out(DEBUG) << fileContents << std::endl;
//
//		// construct the mysql query
//		std::stringstream query;
//
//		if(forceReplaceArg.getValue())
//			query << "REPLACE ";
//		else
//			query << "INSERT ";
//
//		query << "INTO CONFIG_FILES VALUES "
//				<< "('" << configFilePrimaryKey << "' ,"
//				<< "'" << fileDescriptionArg.getValue() << "' ,"
//				<< "'" << fileContents << "') ;";
//
//		streamlog_out(DEBUG) << "Using database DQM4HEP ..." << std::endl;
//		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pDBInterface->execute("USE DQM4HEP ;"));
//
//		streamlog_out(DEBUG) << "Storing config file to db ..." << std::endl;
//		THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pDBInterface->execute(query.str()));
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
