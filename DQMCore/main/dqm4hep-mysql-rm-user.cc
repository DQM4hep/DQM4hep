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

  DBInterface interface;
  unsigned int nRemovedUsers(0);

  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.connect(dbHostArg.getValue(), "root", rootPassword, ""));
  } catch (StatusCodeException &exception) {
    DQM4HEP_NO_EXCEPTION( dqm_error("MySQL error, couldn't connect to host '{0}' as root: {1}", dbHostArg.getValue(), exception.toString()); );
    return 1;
  }

  try {
    std::stringstream query;
    query << "select user,host from mysql.user where user=\"" << dbUserArg.getValue() << "\";";
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=,
                    interface.queryAndHandle(query.str(), [&interface, &nRemovedUsers](MYSQL_RES *result) {
                      MYSQL_ROW row;
                      while ((row = mysql_fetch_row(result))) {
                        std::stringstream dropQuery;
                        dropQuery << "drop user \"" << row[0] << "\"@\"" << row[1] << "\";";
                        dqm_info("Removing user '{0}'@'{1}' ...", row[0], row[1]);
                        if(STATUS_CODE_SUCCESS != interface.execute(dropQuery.str())) {
                          throw StatusCodeException(STATUS_CODE_FAILURE);
                        }
                        ++nRemovedUsers;
                      }
                    }));
  } catch (StatusCodeException &exception) {
    dqm_error("MySQL error, couldn't remove user '{0}': {1}", dbUserArg.getValue(), exception.toString());
    return 1;
  }

  if (0 == nRemovedUsers)
    dqm_info("No user '{0}' on host '{1}'!", dbUserArg.getValue(), dbHostArg.getValue());
  else
    dqm_info("Successfully removed user '{0}' on host '{1}'!", dbUserArg.getValue(), dbHostArg.getValue());

  return 0;
}
