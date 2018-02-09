/// \file dqm4hep-dump-plugins.cc
/*
 *
 * dqm4hep-dump-plugins.cc main source file template automatically generated
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

  TCLAP::ValueArg<std::string> dbUserArg("u", "user", "The database user", false, "DQM4HEP", "string");
  pCommandLine->add(dbUserArg);

  TCLAP::ValueArg<std::string> dbDbArg("d", "database", "The database name", false, "DQM4HEP", "string");
  pCommandLine->add(dbDbArg);

  TCLAP::ValueArg<std::string> dbPasswordArg("p", "password", "The database password", false, "", "string");
  pCommandLine->add(dbPasswordArg);

  TCLAP::ValueArg<std::string> dbTableArg("t", "table", "The database table to dump", true, "", "string");
  pCommandLine->add(dbTableArg);

  TCLAP::ValueArg<std::string> parameterNameArg("n", "parameter-name", "The parameter name", true, "", "string");
  pCommandLine->add(parameterNameArg);

  TCLAP::ValueArg<std::string> parameterValueArg("v", "parameter-value", "The parameter value", true, "", "string");
  pCommandLine->add(parameterValueArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  std::string password = dbPasswordArg.getValue();
  std::string parameter = parameterNameArg.getValue();
  std::string value = parameterValueArg.getValue();
  StringMap parameters;
  parameters[parameter] = value;

  if (parameter.size() > 256) {
    dqm_error("Invalid parameter size, expected <= 256");
    return 1;
  }

  if (password.empty()) {
    char *promptPassword = getpass("Password : ");

    if (promptPassword == nullptr) {
      dqm_error("No password specified !");
      return 1;
    }

    password = promptPassword;
  }

  try {
    DBInterface interface;
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=,
                    interface.connect(dbHostArg.getValue(), dbUserArg.getValue(), password, dbDbArg.getValue()));
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.createParameterTable(dbTableArg.getValue(), true, false));
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.setParameters(dbTableArg.getValue(), parameters));
  } catch (StatusCodeException &exception) {
    dqm_error("MySQL error, couldn't set parameter into table: {0}", exception.toString());
    return 1;
  }

  dqm_info("Successfully set parameter '{0}', value '{1}' into db", parameter, value);

  return 0;
}
