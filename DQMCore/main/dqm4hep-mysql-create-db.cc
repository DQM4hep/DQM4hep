//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/DBInterface.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/DQM4hepConfig.h>

// -- tclap headers
#include "tclap/Arg.h"
#include "tclap/CmdLine.h"

using namespace std;
using namespace dqm4hep::core;

int main(int argc, char *argv[]) {
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  std::unique_ptr<TCLAP::CmdLine> pCommandLine(new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4hep_VERSION_STR));

  TCLAP::ValueArg<std::string> dbHostArg("k", "host", "The database host", false, "localhost", "string");
  pCommandLine->add(dbHostArg);

  TCLAP::ValueArg<std::string> dbDbArg("d", "database", "The database name", false, "DQM4HEP", "string");
  pCommandLine->add(dbDbArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  dqm_info("Will create database '{0}' on host '{1}' ...", dbDbArg.getValue(), dbHostArg.getValue());

  std::stringstream passwordMessage;
  passwordMessage << "MySQL root password [host=\'" << dbHostArg.getValue() << "\']: ";
  const std::string password = getpass(passwordMessage.str().c_str());

  if (password.empty()) {
    dqm_error("No password specified !");
    return 1;
  }

  DBInterface interface;

  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.connect(dbHostArg.getValue(), "root", password, ""));
  } catch (StatusCodeException &exception) {
    DQM4HEP_NO_EXCEPTION( dqm_error("MySQL error, couldn't connect to host '{0}' as root: {1}", dbHostArg.getValue(), exception.toString()); );
    return 1;
  }

  try {
    std::stringstream query;
    query << "CREATE DATABASE IF NOT EXISTS " << dbDbArg.getValue() << " ;";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));
  } catch (StatusCodeException &exception) {
    DQM4HEP_NO_EXCEPTION( dqm_error("MySQL error, couldn't create database '{0}': {1}", dbDbArg.getValue(), exception.toString()); );
    return 1;
  }

  dqm_info("Successfully created database '{0}' on host '{1}' !", dbDbArg.getValue(), dbHostArg.getValue());

  return 0;
}
