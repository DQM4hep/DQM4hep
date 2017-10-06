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
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/Internal.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/MonitorElementManager.h"

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

#define assert_test(Command) \
  if( ! (Command) )\
  {\
    dqm_error( "Assertion failed : {0}, line {1}", #Command, __LINE__ );\
    exit(1);\
  }

int main(int argc, char* argv[])
{
  Logger::createLogger("test-me-mgr", {Logger::coloredConsole()});
  Logger::setMainLogger("test-me-mgr");
  Logger::setLogLevel(spdlog::level::debug);

  std::unique_ptr<MonitorElementManager> meMgr = std::unique_ptr<MonitorElementManager>(new MonitorElementManager());

  MonitorElement *pMonitorElement = nullptr;
  std::vector<MonitorElement*> monitorElements;
  assert_test(STATUS_CODE_SUCCESS == meMgr->bookMonitorElement("TGraph", "/", "TestGraph", pMonitorElement));

  meMgr->getMonitorElements(monitorElements);
  assert_test(monitorElements.size() == 1);
  monitorElements.clear();

  assert_test(STATUS_CODE_ALREADY_PRESENT == meMgr->bookMonitorElement("TGraph", "/", "TestGraph", pMonitorElement));
  assert_test(STATUS_CODE_SUCCESS == meMgr->getMonitorElement("/", "TestGraph", pMonitorElement));
  assert_test(STATUS_CODE_NOT_FOUND == meMgr->getMonitorElement("/", "TestGraph2", pMonitorElement));

  assert_test(STATUS_CODE_SUCCESS == meMgr->removeMonitorElement("/", "TestGraph"));
  assert_test(STATUS_CODE_NOT_FOUND == meMgr->getMonitorElement("/", "TestGraph", pMonitorElement));



  return 0;
}
