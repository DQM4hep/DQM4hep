/// \file test-me-mgr.cc
/*
 *
 * test-me-mgr.cc main source file template automatically generated
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
