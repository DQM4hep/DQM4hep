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
