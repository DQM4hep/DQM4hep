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
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/UnitTesting.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;

class TestPlugin {
public:
  TestPlugin() = default;
  virtual ~TestPlugin() {};
  void print() {
    dqm_info("Hello world plugin !");
  }
};

DQM_PLUGIN_DECL(TestPlugin, "TestPlugin");

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-plugin");

  auto plugin = PluginManager::instance()->create<TestPlugin>("TestPlugin");
  unitTest.test("FOUND_PLUGIN", nullptr != plugin);
  plugin->print();

  return 0;
}
