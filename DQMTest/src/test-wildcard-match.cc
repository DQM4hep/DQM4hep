/// \file test-wildcard-match.cc
/*
 *
 * test-wildcard-match.cc main source file template automatically generated
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
#include <dqm4hep/StatusCodes.h>

using namespace std;
using namespace dqm4hep::core;

#define assert_test(Command)                                                                                           \
  if (!(Command)) {                                                                                                    \
    dqm_error("Assertion failed : {0}, line {1}", #Command, __LINE__);                                                 \
    exit(1);                                                                                                           \
  }

int main(int /*argc*/, char ** /*argv*/) {
  Logger::createLogger("test-wildcard-match", {Logger::coloredConsole()});
  Logger::setMainLogger("test-wildcard-match");
  Logger::setLogLevel(spdlog::level::debug);

  // matching ones
  assert_test(wildcardMatch("tototatatutu", "tototatatutu"));
  assert_test(wildcardMatch("tototatatutu", "toto*"));
  assert_test(wildcardMatch("tototatatutu", "toto*tutu"));
  assert_test(wildcardMatch("tototatatutu", "toto?atatutu"));
  assert_test(wildcardMatch("tototatatutu", "toto?ata*u"));
  assert_test(wildcardMatch("tototatatutu", "*o*a*u"));
  assert_test(wildcardMatch("tototatatutu", "t?t?t?t?t?t?"));

  // not matching ones
  assert_test(not wildcardMatch("tototatatutu", "toto*tututut"));
  assert_test(not wildcardMatch("tototatatutu", "toto?tatatutu"));
  assert_test(not wildcardMatch("tototatatutu", "?o?o?o?ua?a?u*"));
  assert_test(not wildcardMatch("tototatatutu", "a different string"));
  assert_test(not wildcardMatch("tototatatutu", "?tototatatutu"));
  assert_test(not wildcardMatch("tototatatutu", "tototatatutu?"));
  assert_test(not wildcardMatch("tototatatutu", "tototatat??tu"));
  
  return 0;
}
