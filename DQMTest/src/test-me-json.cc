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
#include <dqm4hep/MonitorElementManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/json.h>
#include <dqm4hep/UnitTesting.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-me-json");

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  MonitorElementPtr graphElement;
  unitTest.test("BOOK_GRAPH", STATUS_CODE_SUCCESS == meMgr->bookMonitorElement("TGraph", "/", "TestGraph", graphElement));

  TGraph *graph = graphElement->objectTo<TGraph>();
  unitTest.test("VALID_GRAPH", nullptr != graph);
  graph->SetPoint(0, 12, 45);
  graph->SetPoint(1, 14, 75);
  
  json graphJson = nullptr;
  graphElement->toJson(graphJson);
  unitTest.test("VALID_GRAPH_JSON", nullptr != graphJson);
  unitTest.test("GRAPH_JSON_OBJECT", nullptr != graphJson.value("object", json(nullptr)));
  unitTest.test("GRAPH_JSON_NO_REF", nullptr == graphJson.value("reference", json(nullptr)));
  unitTest.test("GRAPH_JSON_PATH", 0 != graphJson.count("path"));
  
  DQM4HEP_NO_EXCEPTION( dqm_debug(graphJson.dump(2)); );

  return 0;
}
