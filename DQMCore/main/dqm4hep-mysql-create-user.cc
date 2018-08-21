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

  TCLAP::ValueArg<std::string> dbUserArg("u", "user", "The MySQL user to create", true, "", "string");
  pCommandLine->add(dbUserArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  if (dbUserArg.getValue() == "DQM4HEP") {
    dqm_error("MySQL user name DQM4HEP is not a free user name. Please choose an other one !");
    exit(1);
  }

  StringMap hostInfo;
  dqm4hep::core::fillHostInfo(hostInfo);
  const std::string hostName(hostInfo["host"]);

  std::stringstream passwordMessage;
  passwordMessage << "MySQL root password [host=\'" << dbHostArg.getValue() << "\']: ";
  const std::string rootPassword = getpass(passwordMessage.str().c_str());

  if (rootPassword.empty()) {
    dqm_error("No password specified !");
    return 1;
  }

  passwordMessage.str("");
  passwordMessage << "MySQL password for user '" << dbUserArg.getValue() << "': ";
  const std::string userPassword = getpass(passwordMessage.str().c_str());

  if (userPassword.empty()) {
    dqm_error("No password specified !");
    return 1;
  }

  DBInterface interface;

  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.connect(dbHostArg.getValue(), "root", rootPassword, ""));
  } catch (StatusCodeException &exception) {
    dqm_error("MySQL error, couldn't connect to host '{0}' as root: {1}", dbHostArg.getValue(), exception.toString());
    return 1;
  }

  try {
    bool alreadyExists(false);

    std::stringstream query;
    query << "select user from mysql.user where user=\"DQM4HEP\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.queryAndHandle(query.str(), [&alreadyExists](MYSQL_RES *result) {
      alreadyExists = mysql_num_rows(result) > 0;
    }));

    if (!alreadyExists) {
      query.str("");
      query << "CREATE USER \"DQM4HEP\"@\"%\", \"DQM4HEP\"@\"localhost\", \"DQM4HEP\"@\"" << hostName << "\";";
      dqm_info("DQM4HEP user not doesn't exists yet. Creating ...");
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));
    }

    query.str("");
    query << "CREATE USER \"" << dbUserArg.getValue() << "\"@\"%\" IDENTIFIED BY \"" << userPassword << "\","
          << "\"" << dbUserArg.getValue() << "\"@\"localhost\" IDENTIFIED BY \"" << userPassword << "\","
          << "\"" << dbUserArg.getValue() << "\"@\"" << hostName << "\" IDENTIFIED BY \"" << userPassword << "\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));

    query.str("");
    query << "grant all privileges on " << dbDbArg.getValue() << ".* to \"" << dbUserArg.getValue() << "\"@\"%\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));

    query.str("");
    query << "grant all privileges on " << dbDbArg.getValue() << ".* to \"" << dbUserArg.getValue()
          << "\"@\"localhost\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));

    query.str("");
    query << "grant all privileges on " << dbDbArg.getValue() << ".* to \"" << dbUserArg.getValue() << "\"@\""
          << hostName << "\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));

    query.str("");
    query << "grant select on " << dbDbArg.getValue() << ".* to \"DQM4HEP\"@\"%\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));

    query.str("");
    query << "grant select on " << dbDbArg.getValue() << ".* to \"DQM4HEP\"@\"localhost\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));

    query.str("");
    query << "grant select on " << dbDbArg.getValue() << ".* to \"DQM4HEP\"@\"" << hostName << "\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.execute(query.str()));
  } catch (StatusCodeException &exception) {
    dqm_error("MySQL error, couldn't create user '{0}' and permissions for database '{1}': {2}", dbUserArg.getValue(),
              dbDbArg.getValue(), exception.toString());
    return 1;
  }

  dqm_info("Successfully created user '{0}' and set permissions for database '{1}' on host '{2}' !",
           dbUserArg.getValue(), dbDbArg.getValue(), dbHostArg.getValue());

  return 0;
}
