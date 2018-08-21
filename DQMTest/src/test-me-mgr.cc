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
#include <dqm4hep/UnitTesting.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-me-mgr"); 

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  MonitorElementPtr monitorElement;
  MonitorElementList monitorElements;
  unitTest.test("BOOK_GRAPH", STATUS_CODE_SUCCESS == meMgr->bookMonitorElement("TGraph", "/", "TestGraph", monitorElement));

  meMgr->getMonitorElements(monitorElements);
  unitTest.test("STORAGE_SIZE", monitorElements.size() == 1);
  monitorElements.clear();

  unitTest.test("DUPLICATE_BOOKING", STATUS_CODE_ALREADY_PRESENT == meMgr->bookMonitorElement("TGraph", "/", "TestGraph", monitorElement));
  unitTest.test("GRAPH_EXISTS", STATUS_CODE_SUCCESS == meMgr->getMonitorElement("/", "TestGraph", monitorElement));
  unitTest.test("NO_GRAPH2", STATUS_CODE_NOT_FOUND == meMgr->getMonitorElement("/", "TestGraph2", monitorElement));

  unitTest.test("GRAPH_RM", STATUS_CODE_SUCCESS == meMgr->removeMonitorElement("/", "TestGraph"));
  unitTest.test("GRAPH_NOT_FOUND", STATUS_CODE_NOT_FOUND == meMgr->getMonitorElement("/", "TestGraph", monitorElement));

  return 0;
}
