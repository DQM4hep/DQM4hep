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
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/Path.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/XmlHelper.h>
#include <dqm4hep/DQM4hepConfig.h>

// -- tclap headers
#include <tclap/Arg.h>
#include <tclap/CmdLine.h>

// -- std headers
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sstream>

// -- root headers
#include <TDirectory.h>
#include <TFile.h>
#include <TKey.h>
#include <TSystem.h>

using namespace std;
using namespace dqm4hep::core;

//-------------------------------------------------------------------------------------------------

void CreateElementsFromDirectory(const std::string &fileName, TDirectory *pDirectory, const Path &currentPath,
                                 TiXmlElement *pParentElement) {
  TIter iter(pDirectory->GetListOfKeys());
  TKey *pTKey(nullptr);

  while ((pTKey = (TKey *)iter()) != nullptr) {
    pDirectory->cd();

    if (pTKey->IsFolder()) {
      pDirectory->cd(pTKey->GetName());
      TDirectory *pSubDir = gDirectory;
      Path localPath = currentPath + std::string(pTKey->GetName());
      CreateElementsFromDirectory(fileName, pSubDir, localPath, pParentElement);
      pDirectory->cd();
    } else {
      auto pXmlElement = new TiXmlElement("monitorElement");
      pXmlElement->SetAttribute("path", currentPath.getPath());
      pXmlElement->SetAttribute("name", pTKey->GetName());
      auto pQTestsXmlComment = new TiXmlComment("Quality tests to execute ...");
      pXmlElement->LinkEndChild(pQTestsXmlComment);
      pParentElement->LinkEndChild(pXmlElement);
    }
  }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQM4hep_VERSION_STR);

  TCLAP::ValueArg<std::string> rootFileArg("i", "input-root-file", "The root input file", true, "", "string");
  pCommandLine->add(rootFileArg);

  TCLAP::ValueArg<std::string> qtestFileArg("o", "output-qtest-file", "The qtest output file", true, "", "string");
  pCommandLine->add(qtestFileArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  Path fullRootFilePath(rootFileArg.getValue());
  fullRootFilePath = gSystem->GetWorkingDirectory() + fullRootFilePath;

  // Create the output xml document
  TiXmlDocument document;

  auto pRootElement = new TiXmlElement("dqm4hep");
  auto pDeclaration = new TiXmlDeclaration("1.0", "", "");
  auto pQTestsXmlElement = new TiXmlElement("qtests");
  auto pQTestsXmlComment = new TiXmlComment("Create your quality tests here");

  document.LinkEndChild(pDeclaration);
  document.LinkEndChild(pRootElement);
  pRootElement->LinkEndChild(pQTestsXmlElement);
  pQTestsXmlElement->LinkEndChild(pQTestsXmlComment);

  // Populate the xml structure
  auto pTFile = new TFile(fullRootFilePath.getPath().c_str(), "READ");
  Path path("/");
  CreateElementsFromDirectory(fullRootFilePath.getPath(), pTFile, path, pRootElement);

  // save xml output file
  document.SaveFile(qtestFileArg.getValue());

  delete pTFile;

  return 0;
}
