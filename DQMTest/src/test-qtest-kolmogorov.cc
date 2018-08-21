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

void fillTest(TH1 *histogram) {
  histogram->Fill(1);
  histogram->Fill(50);
  histogram->Fill(50);
  histogram->Fill(53);
  histogram->Fill(42);
  histogram->Fill(-2); // underflow
  histogram->Fill(-6); // underflow
  histogram->Fill(102);// overflow
  histogram->Fill(105);// overflow
  histogram->Fill(106);// overflow
}

void fillTest(TGraph *graph) {
  graph->SetPoint(0, 12, 75);
  graph->SetPoint(1, 17, 45);
  graph->SetPoint(2, 25, 73);
  graph->SetPoint(3, 30, 12);
}

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-qtest-kolmogorov");
  
  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());
  
  // create test element TGraph
  MonitorElementPtr testElement;
  meMgr->bookObject<TGraph>("/", "TestGraph", "A test graph", testElement);
  TGraph *graph = testElement->objectTo<TGraph>();
  unitTest.test("BOOK_GRAPH", nullptr != graph);
  fillTest(graph);
  
  // create reference TH1
  PtrHandler<TObject> referenceHistogram(new TH1F("", "A good reference", 100, 0.f, 99.f), true);
  fillTest((TH1F*)referenceHistogram.ptr());

  // create reference TGraph
  PtrHandler<TObject> referenceGraph(new TGraph(), true);
  fillTest((TGraph*)referenceGraph.ptr());

  // configure reporting
  QReportStorage storage; QReport report; json jsonReport;
  
  // creating the qtest XML config
  const std::string qtestType("KolmogorovTest");
  const std::string qtestName("UnitTestKolmogorov");
  TiXmlElement *qtestElement = new TiXmlElement("qtest");
  std::shared_ptr<TiXmlElement> sharedQtest(qtestElement);
  qtestElement->SetAttribute("type", qtestType);
  qtestElement->SetAttribute("name", qtestName);

  // configuring the qtest
  unitTest.test("CREATE_QTEST1", STATUS_CODE_SUCCESS == meMgr->createQualityTest(sharedQtest.get()));
  unitTest.test("ADD_QTEST1", STATUS_CODE_SUCCESS == meMgr->addQualityTest(testElement->path(), testElement->name(), qtestName));

  // invalid test - no reference
  unitTest.test("RUN_QTEST1", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  unitTest.test("GET_REPORT__QTEST1", STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST1", report.m_qualityFlag == INVALID);

  // valid test - same graph
  storage.clear();
  testElement->setReferenceObject(referenceGraph);
  unitTest.test("RUN_QTEST2", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  unitTest.test("GET_REPORT_QTEST2", STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST2", report.m_qualityFlag == SUCCESS);

  // invalid test - object and reference types do not match
  storage.clear();
  testElement->setReferenceObject(referenceHistogram);
  unitTest.test("RUN_QTEST3", STATUS_CODE_SUCCESS == meMgr->runQualityTest(testElement->path(), testElement->name(), qtestName, storage));
  unitTest.test("GET_REPORT_QTEST3", STATUS_CODE_SUCCESS == storage.report(testElement->path(), testElement->name(), qtestName, report));
  report.toJson(jsonReport);
  DQM4HEP_NO_EXCEPTION( std::cout << jsonReport.dump(2) << std::endl; );
  unitTest.test("FLAG_QTEST3", report.m_qualityFlag == INVALID);
  
}
