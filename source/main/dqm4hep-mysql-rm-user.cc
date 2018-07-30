/// \file dqm4hep-mysql-run-user.cc
/*
 *
 * dqm4hep-mysql-run-user.cc main source file template automatically generated
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
 *
 * Create a quality test template file a ROOT file.
 * Parses the different directories and populate the
 * qtest file with found objects.
 */

// -- dqm4hep headers
#include <dqm4hep/DBInterface.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>

// -- tclap headers
#include "tclap/Arg.h"
#include "tclap/CmdLine.h"

using namespace std;
using namespace dqm4hep::core;

int main(int argc, char *argv[]) {
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  std::unique_ptr<TCLAP::CmdLine> pCommandLine(new TCLAP::CmdLine(cmdLineFooter, ' ', DQMCore_VERSION_STR));

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
