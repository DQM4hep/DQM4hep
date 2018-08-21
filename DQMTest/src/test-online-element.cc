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
#include <dqm4hep/OnlineElement.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/UnitTesting.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;
using namespace dqm4hep::online;
using UnitTest = dqm4hep::test::UnitTest;

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-online-element");

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  OnlineElementPtr monitorElement;
  OnlineElementPtrList monitorElements;
  unitTest.test("BOOK_GRAPH", STATUS_CODE_SUCCESS == meMgr->bookMonitorElement("TGraph", "/", "TestGraph", monitorElement));

  meMgr->getMonitorElements(monitorElements);
  unitTest.test("STORAGE_SIZE", monitorElements.size() == 1);
  monitorElements.clear();

  unitTest.test("DUPLICATE_BOOK", STATUS_CODE_ALREADY_PRESENT == meMgr->bookMonitorElement("TGraph", "/", "TestGraph", monitorElement));
  unitTest.test("GET_ELEMENT", STATUS_CODE_SUCCESS == meMgr->getMonitorElement("/", "TestGraph", monitorElement));
  unitTest.test("GET_ELEMENT_NOT_FOUND", STATUS_CODE_NOT_FOUND == meMgr->getMonitorElement("/", "TestGraph2", monitorElement));

  unitTest.test("RM_ELEMENT", STATUS_CODE_SUCCESS == meMgr->removeMonitorElement("/", "TestGraph"));
  unitTest.test("GET_ELEMENT_NOT_FOUND2", STATUS_CODE_NOT_FOUND == meMgr->getMonitorElement("/", "TestGraph", monitorElement));

  return 0;
}
