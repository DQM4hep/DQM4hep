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
#include <dqm4hep/XMLParser.h>
#include <dqm4hep/DQM4hepConfig.h>

// -- tclap headers
#include "tclap/Arg.h"
#include "tclap/CmdLine.h"

using namespace std;
using namespace dqm4hep::core;

struct DBInterfaceMetadata {
  std::shared_ptr<DBInterface> m_dbInterface = {nullptr};
  std::string m_host = {""};
  std::string m_user = {""};
  std::string m_db = {""};
  std::string m_password = {""};
};

typedef std::map<std::string, DBInterfaceMetadata> DBInterfaceMetadataMap;

int main(int argc, char *argv[]) {
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  std::unique_ptr<TCLAP::CmdLine> pCommandLine(new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4hep_VERSION_STR));

  TCLAP::ValueArg<std::string> xmlInputFileArg(
      "f", "parameter-file", "The xml input file describing the parameters and databases to feed", true, "", "string");
  pCommandLine->add(xmlInputFileArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  XMLParser parser;
  parser.parse(xmlInputFileArg.getValue());

  TiXmlDocument &document(parser.document());
  DBInterfaceMetadataMap dbInterfaceMap;

  TiXmlElement *root = document.RootElement();
  TiXmlElement *databases = root->FirstChildElement("databases");

  if (databases == nullptr) {
    dqm_error("ERROR: no <databases> tag found in root element !");
    return 1;
  }

  for (TiXmlElement *db = databases->FirstChildElement("db"); db != nullptr; db = db->NextSiblingElement("db")) {
    const char *dbID = db->Attribute("id");

    if ((dbID == nullptr) || std::string(dbID).empty()) {
      dqm_error("ERROR: <db> tag has no 'id' attribute or is empty, line {0}", db->Row());
      return 1;
    }

    if (dbInterfaceMap.find(dbID) != dbInterfaceMap.end()) {
      dqm_error("ERROR: <db> tag with id '{0}', line {1}, is duplicated !", dbID, db->Row());
      return 1;
    }

    std::string dbHost = db->Attribute("host") != nullptr ? db->Attribute("host") : "localhost";
    std::string dbUser = db->Attribute("user") != nullptr ? db->Attribute("user") : "DQM4HEP";
    std::string dbDb = db->Attribute("db") != nullptr ? db->Attribute("db") : "DQM4HEP";
    std::string dbPassword = db->Attribute("password") != nullptr ? db->Attribute("password") : "";

    DBInterfaceMetadata metadata;
    metadata.m_dbInterface = std::make_shared<DBInterface>();
    metadata.m_host = dbHost;
    metadata.m_user = dbUser;
    metadata.m_db = dbDb;
    metadata.m_password = dbPassword;

    dbInterfaceMap[dbID] = metadata;
  }

  if (dbInterfaceMap.empty()) {
    dqm_error("ERROR: no <db> tag found in <databases> tag !");
    return 1;
  }

  int nParameters(0), nParametersTags(0);
  std::set<std::pair<std::string, std::string>> dbTables;

  for (TiXmlElement *parameters = root->FirstChildElement("parameters"); parameters != nullptr;
       parameters = parameters->NextSiblingElement("parameters")) {
    std::string dbID = parameters->Attribute("db") != nullptr ? parameters->Attribute("db") : "";
    std::string dbTable = parameters->Attribute("table") != nullptr ? parameters->Attribute("table") : "";

    auto findIter = dbInterfaceMap.find(dbID);

    if (dbInterfaceMap.end() == findIter) {
      dqm_error("ERROR: <parameters> tag, db attribute '{0}' not found in <databases> section, line {1}", dbID,
                parameters->Row());
      return 1;
    }

    if (!findIter->second.m_dbInterface->isConnected()) {
      if (findIter->second.m_password.empty()) {
        std::stringstream passwordText;
        passwordText << "Password [host=" << findIter->second.m_host << " db=" << findIter->second.m_db
                     << " user=" << findIter->second.m_user << "] : ";
        char *password = getpass(passwordText.str().c_str());

        if (password == nullptr) {
          dqm_error("ERROR: Invalid password !");
          return 1;
        }

        findIter->second.m_password = password;
      }

      // connect to db
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=,
                      findIter->second.m_dbInterface->connect(findIter->second.m_host, findIter->second.m_user,
                                                              findIter->second.m_password, findIter->second.m_db));
    }

    StringMap parameterMap;
    int nParameterTags(0);

    for (TiXmlElement *parameter = parameters->FirstChildElement("parameter"); parameter != nullptr;
         parameter = parameter->NextSiblingElement("parameter")) {
      const std::string name = parameter->Attribute("name");
      std::string value;

      if (parameter->Attribute("value") != nullptr) {
        value = parameter->Attribute("value");
      } else {
        if (parameter->FirstChild() != nullptr)
          value = parameter->FirstChild()->Value();
      }

      parameterMap[name] = value;
      nParameters++;
      nParameterTags++;
    }

    if (0 == nParameterTags) {
      dqm_error("Tag <parameters> without <parameter> tag inside. Skipping ...");
      continue;
    }

    try {
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=,
                      findIter->second.m_dbInterface->createParameterTable(dbTable, true, false));
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, findIter->second.m_dbInterface->setParameters(dbTable, parameterMap));
    } catch (StatusCodeException &exception) {
      DQM4HEP_NO_EXCEPTION( dqm_error("MySQL error, couldn't push parameters into table: {0}", exception.toString()); );
      return 1;
    }

    std::pair<std::string, std::string> dbIdTable(dbID, dbTable);
    dbTables.insert(dbIdTable);

    nParametersTags++;
  }

  if (0 == nParametersTags) {
    dqm_error("ERROR: No <parameters> tag found !");
    return 1;
  }

  dqm_info("Successfully pushed {0} parameter(s) into {1} db table(s)", nParameters, dbTables.size());

  return 0;
}
