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
    DQM4HEP_NO_EXCEPTION( dqm_error("MySQL error, couldn't set parameter into table: {0}", exception.toString()); );
    return 1;
  }

  dqm_info("Successfully set parameter '{0}', value '{1}' into db", parameter, value);

  return 0;
}
