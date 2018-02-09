/// \file test-plugin.cc
/*
 *
 * test-plugin.cc main source file template automatically generated
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
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/StatusCodes.h>

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

class TestPlugin {
public:
  TestPlugin() = default;
  void print() {
    dqm_info("Hello world plugin !");
  }
};

DQM_PLUGIN_DECL(TestPlugin, "TestPlugin");

int main(int argc, char *argv[]) {
  Logger::createLogger("test-plugin", {Logger::coloredConsole()});
  Logger::setMainLogger("test-plugin");
  Logger::setLogLevel(spdlog::level::debug);

  auto plugin = PluginManager::instance()->create<TestPlugin>("TestPlugin");
  assert_test(nullptr != plugin);
  plugin->print();

  return 0;
}
