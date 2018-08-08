/// \file test-qtest-fit-param-range.cc
/*
 *
 * test-qtest-fit-param-range.cc main source file template automatically generated
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
 * @copyright
 */

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElementManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/PluginManager.h>

#include <TH1.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TRandom.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

#define assert_test(Command)                                                                                           \
  if (!(Command)) {                                                                                                    \
    dqm_error("Assertion failed : {0}, line {1}", #Command, __LINE__);                                                 \
    exit(1);                                                                                                           \
  }

template <typename T>
TiXmlElement *createParameter(const std::string &name, const T &value) {
  TiXmlElement *elt = new TiXmlElement("parameter");
  elt->SetAttribute("name", name);
  elt->SetAttribute("value", typeToString(value));
  return elt;
}

template <typename T>
TiXmlElement *createParameter(const std::string &name, const std::vector<T> &value) {
  TiXmlElement *elt = new TiXmlElement("parameter");
  elt->SetAttribute("name", name);
  std::string valueStr;
  for(auto t : value) valueStr += " " + typeToString(t);
  elt->SetAttribute("value", typeToString(valueStr));
  return elt;
}

TiXmlElement *createQTestXml(
  const std::string &name,
  const std::string &formula,
  int testParameter,
  double deviationLower,
  double deviationUpper,
  const FloatVector &guessParameters = {},
  const FloatVector &functionRange = {},
  bool logLikelihood = false,
  bool pearson = false,
  bool improve = false) {
  
  TiXmlElement *qtestElement = new TiXmlElement("qtest");
  qtestElement->SetAttribute("name", name);
  qtestElement->SetAttribute("type", "FitParamInRangeTest");
  
  qtestElement->LinkEndChild(createParameter("FitFormula", formula));
  qtestElement->LinkEndChild(createParameter("GuessParameters", guessParameters));
  qtestElement->LinkEndChild(createParameter("TestParameter", testParameter));
  qtestElement->LinkEndChild(createParameter("DeviationLower", deviationLower));
  qtestElement->LinkEndChild(createParameter("DeviationUpper", deviationUpper));
  qtestElement->LinkEndChild(createParameter("FunctionRange", functionRange));
  qtestElement->LinkEndChild(createParameter("UseLoglikelihood", logLikelihood));
  qtestElement->LinkEndChild(createParameter("ImproveFitResult", improve));
  qtestElement->LinkEndChild(createParameter("UsePearsonChi2", pearson));
  return qtestElement;
}

int main(int /*argc*/, char ** /*argv*/) {
  
  DQM4HEP_NO_EXCEPTION( Logger::createLogger("test-qtest-fit-parameter", {Logger::coloredConsole()}); );
  Logger::setMainLogger("test-qtest-fit-parameter");
  Logger::setLogLevel(spdlog::level::debug);

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  // create a test elements
  MonitorElementPtr testElement;
  meMgr->bookHisto<TH1F>("/", "TestHisto", "A test histogram", testElement, 101, -10.f, 10.f);
  TH1F *histogram = testElement->objectTo<TH1F>();
  assert_test(nullptr != histogram);
  histogram->FillRandom("gaus", 10000);
  
  MonitorElementPtr testElement2;
  meMgr->bookObject<TGraph>("/", "TestGraph", "A test graph", testElement2);
  TGraph *graph = testElement2->objectTo<TGraph>();
  assert_test(nullptr != graph);
  Double_t errors[100] = {0};
  gRandom->RndmArray(100, errors);
  for(unsigned int i=0 ; i<100 ; i++) {
    double x = i;
    double y = i*0.7 + (errors[i]-0.5)/10;
    graph->SetPoint(i, x, y);
  }
  
  MonitorElementPtr testElement3;
  meMgr->bookObject<TGraph2D>("/", "TestGraph2D", "A test graph 2D", testElement3);
  TGraph2D *graph2D = testElement3->objectTo<TGraph2D>();
  assert_test(nullptr != graph2D);
  Double_t errors2[100] = {0};
  Double_t errors3[100] = {0};
  gRandom->RndmArray(100, errors3);
  gRandom->RndmArray(100, errors2);
  Int_t pointID = 0;
  for(unsigned int i=0 ; i<100 ; i++) {
    for(unsigned int j=0 ; j<100 ; j++) {
      double x = i;
      double y = j;
      double z = i*j*0.7 + ((errors3[i]-0.5)/10.)*((errors2[i]-0.5)/10.);
      graph2D->SetPoint(pointID, x, y, z);
      pointID++;
    }
  }
   
  std::shared_ptr<TiXmlElement> sharedQTest1(createQTestXml(
    "test1", 
    "gaus(0)",
    1,
    -0.5,
    0.5
  ));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest1.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement->path(), testElement->name(), "test1"));
  
  QReportStorage storage; QReport report; json jsonReport;
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), "test1", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), "test1", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS);
  
  std::shared_ptr<TiXmlElement> sharedQTest2(createQTestXml(
    "test2", 
    "dfgaus(0)",
    1,
    -0.5,
    0.5
  ));
  // Can't configure it: Formula is wrong
  assert_test(STATUS_CODE_SUCCESS != meMgr->createQualityTest(sharedQTest2.get()));
  assert_test(STATUS_CODE_SUCCESS != meMgr->addQualityTest(testElement->path(), testElement->name(), "test2"));
  
  std::shared_ptr<TiXmlElement> sharedQTest3(createQTestXml(
    "test3", 
    "gaus(0)",
    5,
    -0.5,
    0.5
  ));
  // Can't configure it: Number of parameter greater than test parameter id
  assert_test(STATUS_CODE_SUCCESS != meMgr->createQualityTest(sharedQTest3.get()));
  assert_test(STATUS_CODE_SUCCESS != meMgr->addQualityTest(testElement->path(), testElement->name(), "test3"));
  
  std::shared_ptr<TiXmlElement> sharedQTest4(createQTestXml(
    "test4",
    "gaus(0)",
    2,
    0.9,
    1.1
  ));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest4.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement->path(), testElement->name(), "test4"));
  
  storage.clear();
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), "test4", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), "test4", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS);
  
  std::shared_ptr<TiXmlElement> sharedQTest5(createQTestXml(
    "test5",
    "[0]*x+[1]",
    0,
    0.6,
    0.8
  ));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest5.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement2->path(), testElement2->name(), "test5"));
  
  std::shared_ptr<TiXmlElement> sharedQTest6(createQTestXml(
    "test6",
    "[0]*x+[1]",
    1,
    -0.1,
    0.1
  ));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest6.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement2->path(), testElement2->name(), "test6"));
  
  storage.clear();
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement2->path(), testElement2->name(), "test6", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement2->path(), testElement2->name(), "test6", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS);
  
  std::shared_ptr<TiXmlElement> sharedQTest7(createQTestXml(
    "test7",
    "[0]*x*y+[1]",
    0,
    0.6,
    0.8
  ));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest7.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement3->path(), testElement3->name(), "test7"));
  
  storage.clear();
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement3->path(), testElement3->name(), "test7", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElement3->path(), testElement3->name(), "test7", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS);
  
  
  return 0;
}
