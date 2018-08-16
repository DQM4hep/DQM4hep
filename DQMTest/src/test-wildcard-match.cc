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
#include <dqm4hep/UnitTesting.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-wildcard-match");

  // matching ones
  unitTest.test("WILDCARD_MATCH1", wildcardMatch("tototatatutu", "tototatatutu"));
  unitTest.test("WILDCARD_MATCH2", wildcardMatch("tototatatutu", "toto*"));
  unitTest.test("WILDCARD_MATCH3", wildcardMatch("tototatatutu", "toto*tutu"));
  unitTest.test("WILDCARD_MATCH4", wildcardMatch("tototatatutu", "toto?atatutu"));
  unitTest.test("WILDCARD_MATCH5", wildcardMatch("tototatatutu", "toto?ata*u"));
  unitTest.test("WILDCARD_MATCH6", wildcardMatch("tototatatutu", "*o*a*u"));
  unitTest.test("WILDCARD_MATCH7", wildcardMatch("tototatatutu", "t?t?t?t?t?t?"));

  // not matching ones
  unitTest.test("WILDCARD_UNMATCH1", not wildcardMatch("tototatatutu", "toto*tututut"));
  unitTest.test("WILDCARD_UNMATCH2", not wildcardMatch("tototatatutu", "toto?tatatutu"));
  unitTest.test("WILDCARD_UNMATCH3", not wildcardMatch("tototatatutu", "?o?o?o?ua?a?u*"));
  unitTest.test("WILDCARD_UNMATCH4", not wildcardMatch("tototatatutu", "a different string"));
  unitTest.test("WILDCARD_UNMATCH5", not wildcardMatch("tototatatutu", "?tototatatutu"));
  unitTest.test("WILDCARD_UNMATCH6", not wildcardMatch("tototatatutu", "tototatatutu?"));
  unitTest.test("WILDCARD_UNMATCH7", not wildcardMatch("tototatatutu", "tototatat??tu"));
  
  return 0;
}
