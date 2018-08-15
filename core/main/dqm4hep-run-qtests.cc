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
#include <dqm4hep/Archiver.h>
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
  static bool firstPrint(true);
  
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
  
  if(firstPrint) {
    std::cout << colors::bold << setw(40) << std::left << "NAME" << setw(30) << std::left << "QTEST" << setw(10) << std::left << "STATUS" << setw(10) << std::left << "QUALITY" << "MESSAGE" << colors::reset << std::endl;
    firstPrint = false;
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
    "o", 
    "root-output", 
    "The root output file to store the processed monitor elements",
    false,
    "", 
    "string");
  pCommandLine->add(rootFileArg);

  TCLAP::ValueArg<std::string> qreportFileArg(
    "q", 
    "qreport-file", 
    "The quality test report output file (json format)", 
    false, 
    "", 
    "string");
  pCommandLine->add(qreportFileArg);

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
  
  StringVector qualityFlags({"undefined", "invalid", "insuf_stat", "success", "warning", "error"});
  std::map<std::string, unsigned int> qualityFlagMap({{"undefined", UNDEFINED}, {"invalid", INVALID}, {"insuf_stat", INSUFFICENT_STAT}, {"success", SUCCESS}, {"warning", WARNING}, {"error", ERROR}});
  TCLAP::ValuesConstraint<std::string> qualityFlagsConstraint(qualityFlags);
  TCLAP::ValueArg<std::string> qualityFlagArg(
    "p", 
    "print-only",
    "Print only the quality reports of the given flag",
    false, 
    "", 
    &qualityFlagsConstraint);
  pCommandLine->add(qualityFlagArg);
  
  TCLAP::SwitchArg writeMonitorElementArg(
    "w", 
    "write-monitor-elements",
    "If set, the monitor elements will also be written in the qreport (if option set)",
    false);
  pCommandLine->add(writeMonitorElementArg);
  
  TCLAP::SwitchArg compressArg(
    "c", 
    "compress-json",
    "If set, the json in the qreport output file will not beautified",
    false);
  pCommandLine->add(compressArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  Logger::setLogLevel(Logger::logLevelFromString(verbosityArg.getValue()));
  bool returnFailure(false);
      
  try {
    // load shared libraries, mainly to get custom quality tests
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PluginManager::instance()->loadLibraries());
    
    // parse xml file
    XMLParser parser;
    parser.parse(qtestFileArg.getValue());
    
    TiXmlDocument &document(parser.document());
    StringMap constants;
    TiXmlElement *rootElement = document.RootElement();
    TiXmlElement *storageElement = rootElement->FirstChildElement("storage");
    std::unique_ptr<MonitorElementManager> monitorElementMgr(new MonitorElementManager());
    
    // create, configure and run quality tests
    QReportStorage reportStorage;
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->parseStorage<MonitorElement>(storageElement));
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->runQualityTests(reportStorage));
    
    const unsigned int qualityExit(qualityExitMap.find(qualityExitArg.getValue())->second);
    const int qualityFlag(qualityFlagArg.isSet() ? qualityFlagMap.find(qualityFlagArg.getValue())->second : -1);
    
    // Print the quality reports in shell        
    for (const auto &iter : reportStorage.reports()) {
      for (const auto &iter2 : iter.second) {

        auto flag = iter2.second.m_qualityFlag;
        
        if(qualityFlag < 0 || qualityFlag == flag) {
          printQReport(iter2.second);          
        }

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
    if (qreportFileArg.isSet()) {
      json root(nullptr), qreport(nullptr), metadata(nullptr), elements(nullptr);
      StringMap hostInfos;
      fillHostInfo(hostInfos);

      // write metadata
      metadata["host"] = hostInfos;
      metadata["date"] = dqm4hep::core::time::asString(dqm4hep::core::time::now());
      root["meta"] = metadata;
      
      // write qreports
      reportStorage.toJson(qreport);
      root["qreports"] = qreport;
      
      ofstream ofile;
      ofile.open(qreportFileArg.getValue());
      
      if(compressArg.getValue()) {
        ofile << root.dump();
      }
      else {
        ofile << root.dump(2);
      }

      ofile.close();
    }
    
    if(rootFileArg.isSet()) {
      Archiver archiver;
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, archiver.open(rootFileArg.getValue(), "RECREATE", true));
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->archive(archiver));
    }
  }
  catch (StatusCodeException &e) {
    DQM4HEP_NO_EXCEPTION( dqm_error("Caught status code exception: {0}", e.toString()); );
    PluginManager::kill();
    return e.getStatusCode();
  } 
  catch (...) {
    dqm_error("Caught unknown exception ...");
    PluginManager::kill();
    return 1;
  }
  
  PluginManager::kill();

  if (returnFailure) {
    dqm_warning("Option --return-on {0} was given => return -1 !", qualityExitArg.getValue());
    return -1;
  }

  return 0;
}
