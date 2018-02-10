/// \file dqm4hep-run-qtests.cc
/*
 *
 * dqm4hep-run-qtests.cc main source file template automatically generated
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
#include <dqm4hep/Internal.h>
#include <dqm4hep/MonitorElementManager.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/XMLParser.h>
#include <dqm4hep/json.h>

// -- tclap headers
#include <tclap/Arg.h>
#include <tclap/CmdLine.h>
#include <tclap/ValuesConstraint.h>

// -- root headers
#include <TDirectory.h>
#include <TFile.h>
#include <TKey.h>
#include <TSystem.h>

using namespace std;
using namespace dqm4hep::core;

namespace colors {
  const std::string underline("\033[4m");
  const std::string redBck("\033[41m");
  const std::string red("\033[31m");
  const std::string green("\033[32m");
  const std::string yellow("\033[33m");
  const std::string cyan("\033[36m");
  const std::string magenta("\033[35m");
  const std::string white("\033[37m");
  const std::string blue("\033[34m");
  const std::string bold("\033[1m");
  const std::string reset("\033[0m");
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

void printQReport(const QReport &report) {
  std::string status, headColor, color;
  
  switch(report.m_qualityFlag) {
    case UNDEFINED: 
      status = "UNDEFINED"; 
      headColor = colors::redBck+colors::white; 
      break;
    case INVALID: 
      status = "INVALID"; 
      headColor = colors::redBck+colors::white; 
      color = colors::red; 
      break;
    case INSUFFICENT_STAT: 
      status = "INSUF_STAT"; 
      color = colors::magenta; 
      break;
    case SUCCESS: 
      status = "SUCCESS"; 
      color = colors::blue; 
      break;
    case WARNING: 
      status = "WARNING"; 
      color = colors::yellow; 
      break;
    case ERROR: 
      status = "ERROR"; 
      color = colors::red; 
      break;
    default: 
      throw StatusCodeException(STATUS_CODE_FAILURE);    
  }
  
  std::cout << headColor << setw(40) << std::left << report.m_monitorElementName << setw(30) << std::left
            << report.m_qualityTestName << color << setw(10) << std::left << status << color << setw(10)
            << std::left << report.m_quality << colors::reset << report.m_message << std::endl;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQMCore_VERSION_STR);

  TCLAP::ValueArg<std::string> qtestFileArg(
    "i", 
    "input-qtest-file", 
    "The qtest input file", 
    true, 
    "", 
    "string");
  pCommandLine->add(qtestFileArg);

  TCLAP::ValueArg<std::string> rootFileArg(
    "r", 
    "input-root-file", 
    "The root input file", 
    true, 
    "", 
    "string");
  pCommandLine->add(rootFileArg);

  TCLAP::ValueArg<std::string> outputJsonFileArg(
    "o", 
    "output-json", 
    "The json output file to store the quality test reports", 
    false, 
    "", 
    "string");
  pCommandLine->add(outputJsonFileArg);

  StringVector verbosities(Logger::logLevels());
  TCLAP::ValuesConstraint<std::string> verbosityConstraint(verbosities);
  TCLAP::ValueArg<std::string> verbosityArg(
    "v", 
    "verbosity", 
    "The logging verbosity", 
    false, 
    "info",
    &verbosityConstraint);
  pCommandLine->add(verbosityArg);

  StringVector qualityExits({"ignore", "failure", "warning", "error"});
  std::map<std::string, unsigned int> qualityExitMap({{"ignore", 0}, {"failure", 1}, {"warning", 2}, {"error", 3}});
  TCLAP::ValuesConstraint<std::string> qualityExitsConstraint(qualityExits);
  TCLAP::ValueArg<std::string> qualityExitArg(
    "e", 
    "exit-on",
    "Returns -1 at end qtest if at least on of the qtest shows errors, warnings or anything not successful",
    false, 
    "ignore", 
    &qualityExitsConstraint);
  pCommandLine->add(qualityExitArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  Logger::setLogLevel(Logger::logLevelFromString(verbosityArg.getValue()));

  const unsigned int qualityExit(qualityExitMap.find(qualityExitArg.getValue())->second);

  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PluginManager::instance()->loadLibraries());
  } catch (StatusCodeException &e) {
    dqm_error("While loading libraries : Caught {0}", e.toString());
    return e.getStatusCode();
  } catch (...) {
    dqm_error("While loading libraries : Caught unknown error");
    return 1;
  }

  const std::string qtestFile(qtestFileArg.getValue());
  const std::string rootFileName(rootFileArg.getValue());

  XMLParser parser;

  try {
    parser.parse(qtestFile);
  } catch (StatusCodeException &e) {
    dqm_error("While reading qtest file : Caught {0}", e.toString());
    return e.getStatusCode();
  }

  TiXmlDocument &document(parser.document());
  StringMap constants;
  TiXmlElement *rootElement = document.RootElement();
  TiXmlElement *qTestsElement = rootElement->FirstChildElement("qtests");

  if (qTestsElement == nullptr) {
    dqm_error("No <qtests> element found in input qtest file '{0}' !", qtestFile);
    return STATUS_CODE_NOT_FOUND;
  }

  std::unique_ptr<MonitorElementManager> monitorElementMgr(new MonitorElementManager());
  std::unique_ptr<TFile> rootFile(new TFile(rootFileName.c_str(), "READ"));

  try {
    for (TiXmlElement *qtest = qTestsElement->FirstChildElement("qtest"); qtest != nullptr;
         qtest = qtest->NextSiblingElement("qtest")) {
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->createQualityTest(qtest));
    }
  } catch (StatusCodeException &e) {
    dqm_error("While creating qtest : Caught {0}", e.toString());
    return e.getStatusCode();
  }

  try {
    for (TiXmlElement *meElt = rootElement->FirstChildElement("monitorElement"); meElt != nullptr;
         meElt = meElt->NextSiblingElement("monitorElement")) {
      std::string path, name, reference;
      THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,
                             XmlHelper::getAttribute(meElt, "path", path));
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(meElt, "name", name));
      THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,
                             XmlHelper::getAttribute(meElt, "reference", reference));

      TObject *pTObject(nullptr);
      Path fullName(path);
      fullName += name;

      // FIXME : bad handling of path in root path
      if (path == "/")
        pTObject = (TObject *)rootFile->Get(name.c_str());
      else
        pTObject = (TObject *)rootFile->Get(fullName.getPath().c_str());

      if (pTObject == nullptr) {
        dqm_error("TObject '{0}' in file '{0}' no found !", fullName.getPath(), rootFileName);
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
      }

      MonitorElementPtr monitorElement;
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->handleMonitorElement(path, pTObject, monitorElement));

      if (!reference.empty()) {
        dqm_debug("Monitor element '{0}' read, reference file '{1}'", name, reference);
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->attachReference(monitorElement, reference));
      }

      for (TiXmlElement *qtest = meElt->FirstChildElement("qtest"); qtest != nullptr;
           qtest = qtest->NextSiblingElement("qtest")) {
        std::string qTestName;
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(qtest, "name", qTestName));
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->addQualityTest(path, name, qTestName));
      }
    }
  } catch (StatusCodeException &e) {
    dqm_error("While creating monitor elements : Caught {0}", e.toString());
    return e.getStatusCode();
  }

  QReportStorage reportStorage;

  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->runQualityTests(reportStorage));
  } catch (StatusCodeException &e) {
    dqm_error("While processing quality tests : Caught {0}", e.toString());
    return e.getStatusCode();
  }

  const QReportContainer &reports(reportStorage.reports());
  bool returnFailure(false);

  // Print the quality reports in shell
  std::cout << colors::bold << setw(40) << std::left << "NAME" << setw(30) << std::left << "QTEST" << setw(10) << std::left
            << "STATUS" << setw(10) << std::left << "QUALITY"
            << "MESSAGE" << colors::reset << std::endl;

  for (const auto &iter : reports) {
    for (const auto &iter2 : iter.second) {
      
      printQReport(iter2.second);
      auto flag = iter2.second.m_qualityFlag;

      if ( (flag == INVALID || flag == UNDEFINED || flag == INSUFFICENT_STAT) && (qualityExit >= 1) ) {
        returnFailure = true;
      } 
      else if(flag == ERROR && qualityExit >= 3) {
        returnFailure = true;
      } 
      else if(flag == WARNING && qualityExit >= 2) {
        returnFailure = true;
      }
    }
  }

  // Save quality reports in a json file
  if (outputJsonFileArg.isSet()) {
    
    json jsonRoot, jsonMetadata, jsonQReports;
    std::string date;
    timeToHMS(time(nullptr), date);
    StringMap hostInfos;
    fillHostInfo(hostInfos);

    jsonMetadata["host"] = hostInfos;
    jsonMetadata["date"] = date;
    jsonRoot["meta"] = jsonMetadata;
    
    for (const auto &iter : reports) {
      for (const auto &iter2 : iter.second) {
        json jsonQReport;
        iter2.second.toJson(jsonQReport);
        jsonQReports.push_back(jsonQReport);
      }
    }
    
    jsonRoot["qreports"] = jsonQReports;
      
    std::ofstream jsonFile;
    jsonFile.open(outputJsonFileArg.getValue().c_str());
    jsonFile << jsonRoot.dump(2);
    jsonFile.close();
  }

  if (returnFailure) {
    dqm_warning("Option --return-on {0} was given => return -1 !", qualityExitArg.getValue());
    return -1;
  }

  return 0;
}
