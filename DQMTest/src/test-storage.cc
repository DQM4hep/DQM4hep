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
#include <dqm4hep/Storage.h>
#include <dqm4hep/UnitTesting.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;

class Object {
public:
  Object(const std::string &oname) : m_name(oname) {
  }
  const std::string &name() {
    return m_name;
  }

private:
  std::string m_name;
};

typedef Directory<Object> Directory_t;
typedef Storage<Object> Storage_t;
typedef Storage_t::ObjectList ObjectList;

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-storage");

  Storage_t storage;

  unitTest.test("MKDIR1", storage.mkdir("/heroes/best") == STATUS_CODE_SUCCESS);
  unitTest.test("MKDIR2", storage.mkdir("/heroes/worst") == STATUS_CODE_SUCCESS);

  ObjectList list;
  storage.getObjects(list);
  unitTest.test("STORAGE_EMPTY", list.empty());

  std::shared_ptr<Object> object;

  unitTest.test("CD_WORKS", storage.cd("/heroes/worst") == STATUS_CODE_SUCCESS);
  storage.add(std::make_shared<Object>("Batman"));
  storage.add(std::make_shared<Object>("SuperLezard"));
  storage.add(std::make_shared<Object>("ConcombreMan"));

  storage.getObjects(list);
  unitTest.test("STORAGE_NOT_EMPTY", list.size() == 3);
  list.clear();

  unitTest.test("PWD1", storage.pwd() == "worst");

  unitTest.test("CD_WORKS2", storage.cd("/heroes/best") == STATUS_CODE_SUCCESS);
  storage.add(std::make_shared<Object>("Superman"));
  storage.add(std::make_shared<Object>("Spiderman"));
  storage.add(std::make_shared<Object>("Me"));

  storage.getObjects(list);
  unitTest.test("STORAGE_NOT_EMPTY2", list.size() == 6);

  unitTest.test("DIR_EXISTS1", storage.dirExists("/heroes/best"));
  unitTest.test("DIR_NOT_EXISTS1", !storage.dirExists("best"));
  unitTest.test("DIR_EXISTS2", storage.dirExists("/heroes"));

  unitTest.test("GO_UP", storage.goUp() == STATUS_CODE_SUCCESS);
  unitTest.test("PWD2", storage.pwd() == "heroes");

  unitTest.test("RMDIR", storage.rmdir("/heroes/worst") == STATUS_CODE_SUCCESS);

  return 0;
}
