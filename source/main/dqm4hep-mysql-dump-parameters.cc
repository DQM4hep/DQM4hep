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
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/DBInterface.h>
#include <dqm4hep/tinyxml.h>

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

using namespace std;
using namespace dqm4hep::core;

struct ParameterEntry
{
  std::string      m_parameter;
  std::string      m_value;
  std::string      m_lastUpdate;
};

typedef std::vector<ParameterEntry> ParameterEntryList; 

int main(int argc, char* argv[])
{
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  std::unique_ptr<TCLAP::CmdLine> pCommandLine(new TCLAP::CmdLine(cmdLineFooter, ' ', DQMCore_VERSION_STR));

  TCLAP::ValueArg<std::string> xmlOutputFileArg(
      "o"
      , "output-file"
      , "The xml output file"
      , false
      , ""
      , "string");
  pCommandLine->add(xmlOutputFileArg);
  
  TCLAP::ValueArg<std::string> dbHostArg(
      "k"
      , "host"
      , "The database host"
      , false
      , "localhost"
      , "string");
  pCommandLine->add(dbHostArg);
  
  TCLAP::ValueArg<std::string> dbDbArg(
      "d"
      , "database"
      , "The database name"
      , false
      , "DQM4HEP"
      , "string");
  pCommandLine->add(dbDbArg);
  
  TCLAP::ValueArg<std::string> dbTableArg(
      "t"
      , "table"
      , "The database table to dump"
      , true
      , ""
      , "string");
  pCommandLine->add(dbTableArg);

  // parse command line
  pCommandLine->parse(argc, argv);
  
  
  DBInterface interface;

  try
  {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.connect(dbHostArg.getValue(), "DQM4HEP", "", dbDbArg.getValue()));
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.dumpParameterTable(dbTableArg.getValue()));
  }
  catch(StatusCodeException &exception)
  {
    dqm_error( "MySQL error, couldn't dump parameters from table: {0}", exception.toString() );
    return 1;
  }
  
  if(xmlOutputFileArg.isSet())
  {
    StringMap parameterMap;
    
    try
    {
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, interface.getTableParameters(dbTableArg.getValue(), parameterMap));
    }
    catch(StatusCodeException &exception)
    {
      dqm_error( "MySQL error, couldn't dump parameters from table: {0}", exception.toString() );
      return 1;
    }
        
    std::string outputFile(xmlOutputFileArg.getValue());      
    TiXmlDocument document;
    
    TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "", "" );
    document.LinkEndChild( decl );
    
    TiXmlElement *root = new TiXmlElement("dqm4hep");
    document.LinkEndChild( root );
    
    // database info
    TiXmlElement *databases = new TiXmlElement("databases");
    root->LinkEndChild( databases );
    
    TiXmlElement *db = new TiXmlElement("db");
    db->SetAttribute("id", "mydb");
    db->SetAttribute("host", dbHostArg.getValue());
    db->SetAttribute("user", "DQM4HEP");
    db->SetAttribute("db", dbDbArg.getValue());
    databases->LinkEndChild( db );

    // parameters
    TiXmlElement *parameters = new TiXmlElement("parameters");
    parameters->SetAttribute("db", "mydb");
    parameters->SetAttribute("table", dbTableArg.getValue());
    root->LinkEndChild( parameters );
    
    for(auto iter : parameterMap)
    {
      TiXmlElement *parameter = new TiXmlElement("parameter");
      parameter->SetAttribute("name", iter.first);
      parameter->SetAttribute("value", iter.second);
      parameters->LinkEndChild(parameter);
    }
    
    document.SaveFile(outputFile);
  }

  return 0;
}
