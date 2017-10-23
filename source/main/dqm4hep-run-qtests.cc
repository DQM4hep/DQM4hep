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
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/XmlHelper.h>
#include <dqm4hep/MonitorElementManager.h>

// -- tclap headers
#include <tclap/CmdLine.h>
#include <tclap/Arg.h>
#include <tclap/ValuesConstraint.h>

// -- root headers
#include <TFile.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <TKey.h>

using namespace std;
using namespace dqm4hep::core;

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


int main(int argc, char* argv[])
{
  std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
  TCLAP::CmdLine *pCommandLine = new TCLAP::CmdLine(cmdLineFooter, ' ', DQMCore_VERSION_STR);

  TCLAP::ValueArg<std::string> qtestFileArg(
      "i"
      , "input-qtest-file"
      , "The qtest input file"
      , true
      , ""
      , "string");
  pCommandLine->add(qtestFileArg);

  TCLAP::ValueArg<std::string> rootFileArg(
      "r"
      , "input-root-file"
      , "The root input file"
      , true
      , ""
      , "string");
  pCommandLine->add(rootFileArg);

  TCLAP::ValueArg<float> lowerQualityLimitArg(
      "l"
      , "lower-qlimit"
      , "Filter qtest output with quality > limit"
      , false
      , 0.f
      , "float");
  pCommandLine->add(lowerQualityLimitArg);

  TCLAP::ValueArg<float> upperQualityLimitArg(
      "u"
      , "upper-qlimit"
      , "Filter qtest output with quality < limit"
      , false
      , 1.f
      , "float");
  pCommandLine->add(upperQualityLimitArg);

  TCLAP::ValueArg<std::string> qualityThresholdsArg(
      "t"
      , "quality-thresolds"
      , "The quality thresholds to apply to get 'good' / 'moderate' / 'bad' qualities (default 0.5:0.8)."
      , false
      , "0.5:0.8"
      , "string");
  pCommandLine->add(qualityThresholdsArg);

  TCLAP::ValueArg<std::string> outputJsonFileArg(
      "o"
      , "output-json"
      , "The json output file to store the quality test reports"
      , false
      , ""
      , "string");
  pCommandLine->add(outputJsonFileArg);

  StringVector verbosities(Logger::logLevels());
  TCLAP::ValuesConstraint<std::string> verbosityConstraint(verbosities);
  TCLAP::ValueArg<std::string> verbosityArg(
      "v"
      , "verbosity"
      , "The logging verbosity"
      , false
      , "info"
      , &verbosityConstraint);
  pCommandLine->add(verbosityArg);

  // parse command line
  pCommandLine->parse(argc, argv);

  Logger::setLogLevel( Logger::logLevelFromString( verbosityArg.getValue() ) );

  StringVector qthresholds;
  tokenize(qualityThresholdsArg.getValue(), qthresholds, ":");

  if(qthresholds.size() != 2)
  {
    dqm_error( "Wrong quality thresholds syntax. Excepted 'number:number' !" );
    return STATUS_CODE_FAILURE;
  }

  float qLimit1(0.), qLimit2(0.);

  if(!stringToType(qthresholds.at(0), qLimit1) || !stringToType(qthresholds.at(1), qLimit2))
  {
    dqm_error( "Wrong quality thresholds syntax. Excepted 'number:number' !" );
    return STATUS_CODE_FAILURE;
  }

  if(qLimit1 < 0 || qLimit2 < 0 || qLimit1 > 1.f || qLimit1 > 1.f)
  {
    dqm_error( "Wrong quality thresholds. Excepted numbers between 0 and 1 !" );
    return STATUS_CODE_FAILURE;
  }

  if(qLimit1 > qLimit2)
    std::swap(qLimit1, qLimit2);

  try
  {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PluginManager::instance()->loadLibraries());
  }
  catch(StatusCodeException &e)
  {
    dqm_error( "While loading libraries : Caught {0}", e.toString() );
    return e.getStatusCode();
  }
  catch(...)
  {
    dqm_error( "While loading libraries : Caught unknown error" );
    return 1;
  }

  const std::string qtestFile(qtestFileArg.getValue());
  const std::string rootFileName(rootFileArg.getValue());
  TiXmlDocument document; StringMap constants;

  try
  {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readXmlFile(qtestFile, document, constants));
  }
  catch(StatusCodeException &e)
  {
    dqm_error( "While reading qtest file : Caught {0}", e.toString() );
    return e.getStatusCode();
  }

  TiXmlElement *rootElement = document.RootElement();
  TiXmlElement *qTestsElement = rootElement->FirstChildElement("qtests");

  if(!qTestsElement)
  {
    dqm_error( "No <qtests> element found in input qtest file '{0}' !", qtestFile );
    return STATUS_CODE_NOT_FOUND;
  }

  std::unique_ptr<MonitorElementManager> monitorElementMgr(new MonitorElementManager());
  std::unique_ptr<TFile> rootFile(new TFile(rootFileName.c_str(), "READ"));

  try
  {
    for(TiXmlElement *qtest = qTestsElement->FirstChildElement("qtest") ; qtest ; qtest = qtest->NextSiblingElement("qtest"))
    {
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->createQualityTest(qtest));
    }
  }
  catch(StatusCodeException &e)
  {
    dqm_error( "While creating qtest : Caught {0}", e.toString() );
    return e.getStatusCode();
  }

  try
  {
    for(TiXmlElement *monitorElement = rootElement->FirstChildElement("monitorElement") ; monitorElement ; monitorElement = monitorElement->NextSiblingElement("monitorElement"))
    {
      std::string path, name, reference;
      THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,
        XmlHelper::getAttribute(monitorElement, "path", path));
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(monitorElement, "name", name));
      THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=,
        XmlHelper::getAttribute(monitorElement, "reference", reference));

      TObject *pTObject(nullptr);
      Path fullName(path); fullName += name;

      // FIXME : bad handling of path in root path
      if(path == "/")
        pTObject = (TObject *) rootFile->Get(name.c_str());
      else
        pTObject = (TObject *) rootFile->Get(fullName.getPath().c_str());

      if(!pTObject)
      {
        dqm_error( "TObject '{0}' in file '{0}' no found !", fullName.getPath(), rootFileName );
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
      }

      MonitorElement *pMonitorElement(nullptr);
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->handleMonitorElement(path, pTObject, pMonitorElement));

      if(!reference.empty())
      {
        dqm_debug( "Monitor element '{0}' read, reference file '{1}'", name, reference );
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->attachReference(pMonitorElement, reference));
      }

      for(TiXmlElement *qtest = monitorElement->FirstChildElement("qtest") ; qtest ; qtest = qtest->NextSiblingElement("qtest"))
      {
        std::string qTestName;
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(qtest, "name", qTestName));
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->addQualityTest(path, name, qTestName));
      }
    }
  }
  catch(StatusCodeException &e)
  {
    dqm_error( "While creating monitor elements : Caught {0}", e.toString() );
    return e.getStatusCode();
  }

  QReportStorage reportStorage;

  try
  {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, monitorElementMgr->runQualityTests(reportStorage));
  }
  catch(StatusCodeException &e)
  {
    dqm_error( "While processing quality tests : Caught {0}", e.toString() );
    return e.getStatusCode();
  }

  const QReportContainer &reports(reportStorage.reports());

  Json::Value jsonRoot;
  Json::Value jsonMetadata; Json::Value jsonHostInfo; Json::Value jsonQReports(Json::arrayValue);
  std::string date; timeToHMS(time(0), date);
  StringMap hostInfos; fillHostInfo(hostInfos);
  for(auto iter : hostInfos) jsonHostInfo[iter.first] = iter.second;

  jsonMetadata["host"] = jsonHostInfo;
  jsonMetadata["date"] = date;
  jsonRoot["meta"] = jsonMetadata;

  unsigned int jsonIndex(0);

  std::cout << bold << setw(40) << std::left << "NAME" << setw(30) << std::left << "QTEST" << setw(10) << std::left << "STATUS" << setw(10) << std::left << "QUALITY" << "MESSAGE" << reset << std::endl;

  for(const auto &iter : reports)
  {
    for(const auto &iter2 : iter.second)
    {
      Json::Value jsonQReport;
      iter2.second.toJson(jsonQReport);
      jsonQReports[jsonIndex] = jsonQReport;
      jsonIndex++;

      if(!iter2.second.m_isSuccessful)
      {
        std::cout << redBck << white << setw(40) << std::left << iter.first.second << setw(30) << std::left << iter2.second.m_qualityTestName << setw(10) << std::left << "FAIL" << setw(10) << std::left << "none" << iter2.second.m_message << reset << std::endl;
      }
      else
      {
        const float quality(iter2.second.m_quality);

        if(quality < lowerQualityLimitArg.getValue() || quality > upperQualityLimitArg.getValue())
          continue;

        if(quality < qLimit1)
        {
          std::cout << setw(40) << std::left << iter.first.second << setw(30) << std::left << iter2.second.m_qualityTestName << blue << setw(10) << std::left << "SUCCESS" << red << setw(10) << std::left << quality << reset << iter2.second.m_message << std::endl;
        }
        else if(qLimit1 <= quality && quality < qLimit2)
        {
          std::cout << setw(40) << std::left << iter.first.second << setw(30) << std::left << iter2.second.m_qualityTestName << blue << setw(10) << std::left << "SUCCESS" << yellow << setw(10) << std::left << quality << reset << iter2.second.m_message << std::endl;
        }
        else
        {
          std::cout << setw(40) << std::left << iter.first.second << setw(30) << std::left << iter2.second.m_qualityTestName << blue << setw(10) << std::left << "SUCCESS" << blue << setw(10) << std::left << quality << reset << iter2.second.m_message << std::endl;
        }
      }
    }
  }

  jsonRoot["qreports"] = jsonQReports;

  if(outputJsonFileArg.isSet())
  {
    const std::string jsonFileName(outputJsonFileArg.getValue());
    Json::StyledWriter writer;
    std::ofstream jsonFile;
    jsonFile.open(jsonFileName.c_str());
    jsonFile << writer.write(jsonRoot);
    jsonFile.close();
  }

  return 0;
}
