/// \file test-qtest-exact-ref-comp.cc
/*
 *
 * test-qtest-exact-ref-comp.cc main source file template automatically generated
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
 */

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElementManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/PluginManager.h>

#include <TH1.h>
#include <TGraph.h>
#include <TGraphErrors.h>

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

TiXmlElement *createQTestXml(
  const std::string &name,
  std::string property,
  std::string method,
  float expectedValue,
  float deviationLower,
  float deviationUpper) {
  
  TiXmlElement *qtestElement = new TiXmlElement("qtest");
  qtestElement->SetAttribute("name", name);
  qtestElement->SetAttribute("type", "PropertyWithinExpectedTest");
  
  qtestElement->LinkEndChild(createParameter("Property", property));
  qtestElement->LinkEndChild(createParameter("Method", method));
  qtestElement->LinkEndChild(createParameter("ExpectedValue", expectedValue));
  qtestElement->LinkEndChild(createParameter("DeviationLower", deviationLower));
  qtestElement->LinkEndChild(createParameter("DeviationUpper", deviationUpper));
  return qtestElement;
}

TiXmlElement *createQTestXml(
  const std::string &name,
  std::string property,
  std::string method,
  float limit) {
  
  TiXmlElement *qtestElement = new TiXmlElement("qtest");
  qtestElement->SetAttribute("name", name);
  qtestElement->SetAttribute("type", "PropertyWithinExpectedTest");
  qtestElement->LinkEndChild(createParameter("Property", property));
  qtestElement->LinkEndChild(createParameter("Method", method));

  if (method == "LowerThan") {
    qtestElement->LinkEndChild(createParameter("DeviationUpper", limit));
  }
  else if (method == "HigherThan") {
    qtestElement->LinkEndChild(createParameter("DeviationUpper", limit));
  }
  else {
    dqm_error("Error in unit test: creating the qtestXml for test {0} failed", name);
    throw StatusCodeException(STATUS_CODE_FAILURE);
  }
  return qtestElement;
}

void fillTest(TH1 *histogram) {
  // mean 10, RMS 4
  histogram->Fill(2);
  histogram->Fill(5);
  histogram->Fill(6);
  histogram->Fill(8);
  histogram->Fill(8);
  histogram->Fill(10);
  histogram->Fill(10);
  histogram->Fill(10);
  histogram->Fill(10);
  histogram->Fill(12);
  histogram->Fill(12);
  histogram->Fill(14);
  histogram->Fill(15);
  histogram->Fill(18);
}

void fillTest(TGraph *graph) {
  graph->SetPoint( 0,  2, 0);
  graph->SetPoint( 1,  5, 1);
  graph->SetPoint( 2,  6, 2);
  graph->SetPoint( 3,  8, 3);
  graph->SetPoint( 4,  8, 3);
  graph->SetPoint( 5, 10, 4);
  graph->SetPoint( 6, 10, 5);
  graph->SetPoint( 7, 10, 5);
  graph->SetPoint( 8, 10, 4);
  graph->SetPoint( 9, 12, 3);
  graph->SetPoint(10, 12, 3);
  graph->SetPoint(11, 14, 2);
  graph->SetPoint(12, 15, 1);
  graph->SetPoint(13, 18, 0);
}

int main(int /*argc*/, char ** /*argv*/) {
  
  DQM4HEP_NO_EXCEPTION( Logger::createLogger("test-qtest-property-within-exp", {Logger::coloredConsole()}); );
  Logger::setMainLogger("test-qtest-property-within-exp");
  Logger::setLogLevel(spdlog::level::debug);

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  // create test element TH1
  MonitorElementPtr testElementTH1;
  meMgr->bookHisto<TH1F>("/", "TestHisto", "A test histogram", testElementTH1, 21, 0.f, 20.f);
  TH1F *histogram = testElementTH1->objectTo<TH1F>();
  assert_test(nullptr != histogram);
  fillTest(histogram);
  
  // create test element TGraph
  MonitorElementPtr testElementTGraph;
  meMgr->bookObject<TGraph>("/", "TestGraph", "A test graph", testElementTGraph);
  TGraph *graph = testElementTGraph->objectTo<TGraph>();
  assert_test(nullptr != graph);
  fillTest(graph);
  
  //
  // TH1 Tests
  //

  // configure reporting
  QReportStorage storage; QReport report; json jsonReport;
  
  // valid test
  std::shared_ptr<TiXmlElement> sharedQTest1(createQTestXml("test1", "Mean", "WithinRange", 10, 8, 12));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest1.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTH1->path(), testElementTH1->name(), "test1"));
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTH1->path(), testElementTH1->name(), "test1", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElementTH1->path(), testElementTH1->name(), "test1", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS);

  // valid test but property is not within range
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest2(createQTestXml("test2", "Mean", "WithinRange", 20, 18, 22));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest2.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTH1->path(), testElementTH1->name(), "test2"));
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTH1->path(), testElementTH1->name(), "test2", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElementTH1->path(), testElementTH1->name(), "test2", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == ERROR);

  // invalid test - unrecognised test type
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest3(createQTestXml("test3", "Foo", "WithinRange", 10, 8, 12));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest3.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTH1->path(), testElementTH1->name(), "test3"));
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTH1->path(), testElementTH1->name(), "test3", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElementTH1->path(), testElementTH1->name(), "test3", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID);

  // invalid test - unrecognised test method
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest4(createQTestXml("test4", "Mean", "Bar", 10, 8, 12));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest4.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTH1->path(), testElementTH1->name(), "test4"));
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTH1->path(), testElementTH1->name(), "test4", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElementTH1->path(), testElementTH1->name(), "test4", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID);

  //
  // TGraph Tests
  //

  // valid test
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest5(createQTestXml("test5", "Mean", "WithinRange", 2.5, 2.0, 3.0));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest5.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test5"));
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test5", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElementTGraph->path(), testElementTGraph->name(), "test5", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == SUCCESS);

  // valid test but property is not within range
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest6(createQTestXml("test6", "Mean", "WithinRange", 5, 4.5, 5.5));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest6.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test6"));
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test6", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElementTGraph->path(), testElementTGraph->name(), "test6", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == ERROR);

  // invalid test - unrecognised test type
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest7(createQTestXml("test7", "Foo", "WithinRange", 2.5, 2.0, 3.0));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest7.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test7"));
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test7", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElementTGraph->path(), testElementTGraph->name(), "test7", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID);

  // invalid test - unrecognised test method
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest8(createQTestXml("test8", "Mean", "Bar", 2.5, 2.0, 3.0));
  assert_test(STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest8.get()));
  assert_test(STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test8"));
  assert_test(STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test8", storage));
  assert_test(STATUS_CODE_SUCCESS == storage.report(testElementTGraph->path(), testElementTGraph->name(), "test8", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  assert_test(report.m_qualityFlag == INVALID);

  return 0;
}
