/// \file test-me-json.cc
/*
 *
 * test-me-json.cc main source file template automatically generated
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
