//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : T.Coates, R.Ete
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElementManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/UnitTesting.h>

#include <TH1.h>
#include <TGraph.h>
#include <TGraphErrors.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;
  
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
  UnitTest unitTest("test-qtest-property-within-exp");

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  // create test element TH1
  MonitorElementPtr testElementTH1;
  meMgr->bookHisto<TH1F>("/", "TestHisto", "A test histogram", testElementTH1, 21, 0.f, 20.f);
  TH1F *histogram = testElementTH1->objectTo<TH1F>();
  unitTest.test("BOOK_HISTO", nullptr != histogram);
  fillTest(histogram);
  
  // create test element TGraph
  MonitorElementPtr testElementTGraph;
  meMgr->bookObject<TGraph>("/", "TestGraph", "A test graph", testElementTGraph);
  TGraph *graph = testElementTGraph->objectTo<TGraph>();
  unitTest.test("BOOK_GRAPH", nullptr != graph);
  fillTest(graph);
  
  //
  // TH1 Tests
  //

  // configure reporting
  QReportStorage storage; QReport report; json jsonReport;
  
  // valid test
  std::shared_ptr<TiXmlElement> sharedQTest1(createQTestXml("test1", "Mean", "WithinRange", 10, 8, 12));
  unitTest.test("CREATE_QTEST1", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest1.get()));
  unitTest.test("ADD_QTEST1", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTH1->path(), testElementTH1->name(), "test1"));
  unitTest.test("RUN_QTEST1", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTH1->path(), testElementTH1->name(), "test1", storage));
  unitTest.test("GET_REPORT_QTEST1", STATUS_CODE_SUCCESS == storage.report(testElementTH1->path(), testElementTH1->name(), "test1", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST1", report.m_qualityFlag == SUCCESS);

  // valid test but property is not within range
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest2(createQTestXml("test2", "Mean", "WithinRange", 20, 18, 22));
  unitTest.test("CREATE_QTEST2", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest2.get()));
  unitTest.test("ADD_QTEST2", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTH1->path(), testElementTH1->name(), "test2"));
  unitTest.test("RUN_QTEST2", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTH1->path(), testElementTH1->name(), "test2", storage));
  unitTest.test("GET_REPORT_QTEST2", STATUS_CODE_SUCCESS == storage.report(testElementTH1->path(), testElementTH1->name(), "test2", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST2", report.m_qualityFlag == ERROR);

  // invalid test - unrecognised test type
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest3(createQTestXml("test3", "Foo", "WithinRange", 10, 8, 12));
  unitTest.test("CREATE_QTEST3", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest3.get()));
  unitTest.test("ADD_QTEST3", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTH1->path(), testElementTH1->name(), "test3"));
  unitTest.test("RUN_QTEST3", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTH1->path(), testElementTH1->name(), "test3", storage));
  unitTest.test("GET_REPORT_QTEST3", STATUS_CODE_SUCCESS == storage.report(testElementTH1->path(), testElementTH1->name(), "test3", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST3", report.m_qualityFlag == INVALID);

  // invalid test - unrecognised test method
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest4(createQTestXml("test4", "Mean", "Bar", 10, 8, 12));
  unitTest.test("CREATE_QTEST4", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest4.get()));
  unitTest.test("ADD_QTEST4", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTH1->path(), testElementTH1->name(), "test4"));
  unitTest.test("RUN_QTEST4", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTH1->path(), testElementTH1->name(), "test4", storage));
  unitTest.test("GET_REPORT_QTEST4", STATUS_CODE_SUCCESS == storage.report(testElementTH1->path(), testElementTH1->name(), "test4", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST4", report.m_qualityFlag == INVALID);

  //
  // TGraph Tests
  //

  // valid test
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest5(createQTestXml("test5", "Mean", "WithinRange", 2.5, 2.0, 3.0));
  unitTest.test("CREATE_QTEST5", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest5.get()));
  unitTest.test("ADD_QTEST5", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test5"));
  unitTest.test("RUN_QTEST5", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test5", storage));
  unitTest.test("GET_REPORT_QTEST5", STATUS_CODE_SUCCESS == storage.report(testElementTGraph->path(), testElementTGraph->name(), "test5", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST5", report.m_qualityFlag == SUCCESS);

  // valid test but property is not within range
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest6(createQTestXml("test6", "Mean", "WithinRange", 5, 4.5, 5.5));
  unitTest.test("CREATE_QTEST6", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest6.get()));
  unitTest.test("ADD_QTEST6", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test6"));
  unitTest.test("RUN_QTEST6", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test6", storage));
  unitTest.test("GET_REPORT_QTEST6", STATUS_CODE_SUCCESS == storage.report(testElementTGraph->path(), testElementTGraph->name(), "test6", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST6", report.m_qualityFlag == ERROR);

  // invalid test - unrecognised test type
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest7(createQTestXml("test7", "Foo", "WithinRange", 2.5, 2.0, 3.0));
  unitTest.test("CREATE_QTEST7", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest7.get()));
  unitTest.test("ADD_QTEST7", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test7"));
  unitTest.test("RUN_QTEST7", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test7", storage));
  unitTest.test("GET_REPORT_QTEST7", STATUS_CODE_SUCCESS == storage.report(testElementTGraph->path(), testElementTGraph->name(), "test7", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST7", report.m_qualityFlag == INVALID);

  // invalid test - unrecognised test method
  storage.clear();
  std::shared_ptr<TiXmlElement> sharedQTest8(createQTestXml("test8", "Mean", "Bar", 2.5, 2.0, 3.0));
  unitTest.test("CREATE_QTEST8", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQTest8.get()));
  unitTest.test("ADD_QTEST8", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test8"));
  unitTest.test("RUN_QTEST8", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElementTGraph->path(), testElementTGraph->name(), "test8", storage));
  unitTest.test("GET_REPORT_QTEST8", STATUS_CODE_SUCCESS == storage.report(testElementTGraph->path(), testElementTGraph->name(), "test8", report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST8", report.m_qualityFlag == INVALID);

  return 0;
}
