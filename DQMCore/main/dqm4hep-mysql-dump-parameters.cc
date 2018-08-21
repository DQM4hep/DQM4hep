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
#include <dqm4hep/tinyxml.h>
#include <dqm4hep/DQM4hepConfig.h>

// -- tclap headers
#include "tclap/Arg.h"
#include "tclap/CmdLine.h"

using namespace std;
using namespace dqm4hep::core;

struct ParameterEntry {
  std::string m_parameter;
  std::string m_value;
  std::string m_lastUpdate;
};

typedef std::vector<ParameterEntry> ParameterEntryList;

int main(int argc, char *argv[]) {
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  std::unique_ptr<TCLAP::CmdLine> pCommandLine(new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4hep_VERSION_STR));

  TCLAP::ValueArg<std::string> xmlOutputFileArg("o", "output-file", "The xml output file", false, "", "string");
  pCommandLine->add(xmlOutputFileArg);

  TCLAP::ValueArg<std::string> dbHostArg("k", "host", "The database host", false, "localhost", "string");
  pCommandLine->add(dbHostArg);

  TCLAP::ValueArg<std::string> dbDbArg("d", "database", "The database name", false, "DQM4HEP", "string");
  pCommandLine->add(dbDbArg);

  TCLAP::ValueArg<std::string> dbTableArg("t", "table", "The database table to dump", true, "", "string");
  pCommandLine->add(dbTableArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  DBInterface interface;

  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=,
                    interface.connect(dbHostArg.getValue(), "DQM4HEP", "", dbDbArg.getValue()));
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.dumpParameterTable(dbTableArg.getValue()));
  } catch (StatusCodeException &exception) {
    DQM4HEP_NO_EXCEPTION( dqm_error("MySQL error, couldn't dump parameters from table: {0}", exception.toString()); );
    return 1;
  }

  if (xmlOutputFileArg.isSet()) {
    StringMap parameterMap;

    try {
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.getTableParameters(dbTableArg.getValue(), parameterMap));
    } catch (StatusCodeException &exception) {
      dqm_error("MySQL error, couldn't dump parameters from table: {0}", exception.toString());
      return 1;
    }

    std::string outputFile(xmlOutputFileArg.getValue());
    TiXmlDocument document;

    auto decl = new TiXmlDeclaration("1.0", "", "");
    document.LinkEndChild(decl);

    auto root = new TiXmlElement("dqm4hep");
    document.LinkEndChild(root);

    // database info
    auto databases = new TiXmlElement("databases");
    root->LinkEndChild(databases);

    auto db = new TiXmlElement("db");
    db->SetAttribute("id", "mydb");
    db->SetAttribute("host", dbHostArg.getValue());
    db->SetAttribute("user", "DQM4HEP");
    db->SetAttribute("db", dbDbArg.getValue());
    databases->LinkEndChild(db);

    // parameters
    auto parameters = new TiXmlElement("parameters");
    parameters->SetAttribute("db", "mydb");
    parameters->SetAttribute("table", dbTableArg.getValue());
    root->LinkEndChild(parameters);

    for (auto iter : parameterMap) {
      auto parameter = new TiXmlElement("parameter");
      parameter->SetAttribute("name", iter.first);
      parameter->SetAttribute("value", iter.second);
      parameters->LinkEndChild(parameter);
    }

    document.SaveFile(outputFile);
  }

  return 0;
}
