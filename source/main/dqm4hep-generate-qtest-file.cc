/// \file dqm4hep-generate-qtest-file.cc
/*
 *
 * dqm4hep-generate-qtest-file.cc main source file template automatically generated
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
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/XmlHelper.h"
#include "dqm4hep/Path.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

// -- std headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>

#include <TFile.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <TKey.h>

using namespace std;
using namespace dqm4hep::core;

//-------------------------------------------------------------------------------------------------

void CreateElementsFromDirectory(const std::string &fileName, TDirectory *pDirectory, const Path &currentPath, TiXmlElement *pParentElement)
{
  TIter iter(pDirectory->GetListOfKeys());
  TKey *pTKey(0);
  
  while( (pTKey = (TKey*) iter()) )
  {
    pDirectory->cd();
    
    if(pTKey->IsFolder())
    {
      pDirectory->cd(pTKey->GetName());
      TDirectory *pSubDir = gDirectory;
      Path localPath = currentPath + std::string(pTKey->GetName());
      CreateElementsFromDirectory(fileName, pSubDir, localPath, pParentElement);
      pDirectory->cd();
    }
    else
    {
      TiXmlElement *pXmlElement = new TiXmlElement("fileElement");
      pXmlElement->SetAttribute("file", fileName);
      pXmlElement->SetAttribute("path", currentPath.getPath());
      pXmlElement->SetAttribute("name", pTKey->GetName());
      TiXmlComment *pQTestsXmlComment = new TiXmlComment("Quality tests to execute ...");
      pXmlElement->LinkEndChild(pQTestsXmlComment);
      pParentElement->LinkEndChild(pXmlElement);
    }
  }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQMCore_VERSION_STR);

  TCLAP::ValueArg<std::string> rootFileArg(
      "i"
      , "input-root-file"
      , "The root input file"
      , true
      , ""
      , "string");
  pCommandLine->add(rootFileArg);
  
  TCLAP::ValueArg<std::string> qtestFileArg(
      "o"
      , "output-qtest-file"
      , "The qtest output file"
      , true
      , ""
      , "string");
  pCommandLine->add(qtestFileArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  Path fullRootFilePath(rootFileArg.getValue());
  fullRootFilePath = std::string(gSystem->GetWorkingDirectory()) + fullRootFilePath;
  
  // Create the output xml document
  TiXmlDocument document;
  
  TiXmlElement *pRootElement = new TiXmlElement("dqm4hep");
  TiXmlDeclaration *pDeclaration = new TiXmlDeclaration( "1.0", "", "" );
  TiXmlElement *pQTestsXmlElement = new TiXmlElement("qtests");
  TiXmlComment *pQTestsXmlComment = new TiXmlComment("Create your quality tests here");
  
  document.LinkEndChild(pDeclaration);
  document.LinkEndChild(pRootElement);
  pRootElement->LinkEndChild(pQTestsXmlElement);
  pQTestsXmlElement->LinkEndChild(pQTestsXmlComment);
  
  // Populate the xml structure
  TFile *pTFile = new TFile(fullRootFilePath.getPath().c_str(), "READ");
  Path path("/");
  CreateElementsFromDirectory(fullRootFilePath.getPath(), pTFile, path, pRootElement);
  
  // save xml output file
  document.SaveFile(qtestFileArg.getValue());
  
  delete pTFile;

  return 0;
}
