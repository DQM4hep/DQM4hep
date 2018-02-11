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

int main(int argc, char *argv[]) {
  Logger::createLogger("test-me-json", {Logger::coloredConsole()});
  Logger::setMainLogger("test-me-json");
  Logger::setLogLevel(spdlog::level::debug);

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  MonitorElementPtr graphElement;
  assert_test(STATUS_CODE_SUCCESS == meMgr->bookMonitorElement("TGraph", "/", "TestGraph", graphElement));

  TGraph *graph = graphElement->objectTo<TGraph>();
  assert_test(nullptr != graph);
  graph->SetPoint(0, 12, 45);
  graph->SetPoint(1, 14, 75);
  
  json graphJson = nullptr;
  graphElement->toJson(graphJson);
  assert_test(nullptr != graphJson);
  assert_test(nullptr != graphJson.value("object", json(nullptr)));
  assert_test(nullptr == graphJson.value("reference", json(nullptr)));
  assert_test(0 != graphJson.count("path"));
  
  dqm_debug(graphJson.dump(2));

  return 0;
}
