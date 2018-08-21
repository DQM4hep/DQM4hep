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
#include <dqm4hep/Directory.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/StatusCodes.h>
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

int main(int /*argc*/, char ** /*argv*/) {
  UnitTest unitTest("test-directory");

  auto root = Directory_t::make_shared("root");
  auto heroes = root->mkdir("heroes");

  std::shared_ptr<Object> object;
  object = std::make_shared<Object>("superman");
  heroes->add(object);
  object = std::make_shared<Object>("spiderman");
  heroes->add(object);
  object = std::make_shared<Object>("batman");
  heroes->add(object);
  unitTest.test("ADD_TEST", heroes->contents().size() == 3);

  heroes->remove(object);
  unitTest.test("RM", heroes->contents().size() == 2);

  root->clear();
  unitTest.test("CLEAR", root->isEmpty());

  auto pBibouDir = root->mkdir("bibou");
  pBibouDir->add(std::make_shared<Object>("toto"));
  pBibouDir->add(std::make_shared<Object>("tata"));
  pBibouDir->add(std::make_shared<Object>("tutu"));
  unitTest.test("ADD2", pBibouDir->contents().size() == 3);

  pBibouDir->remove([](std::shared_ptr<Object> o) { return o->name() == "toto"; });
  unitTest.test("RM2", pBibouDir->contents().size() == 2);
  Path bibouPath = pBibouDir->fullPath();
  unitTest.test("PATH", bibouPath.getPath() == "/root/bibou");
  unitTest.test("NOT_ROOT", !pBibouDir->isRoot());
  unitTest.test("IS_ROOT", root->isRoot());
  unitTest.test("NOT_EMPTY_ROOT", !root->isEmpty());
  unitTest.test("NOT_EMPTY_DIR", !pBibouDir->isEmpty());
  unitTest.test("CONTAINS", pBibouDir->contains([](std::shared_ptr<Object> o) { return o->name() == "tata"; }));
  unitTest.test("NOT_CONTAINS", !pBibouDir->contains([](std::shared_ptr<Object> o) { return o->name() == "teufteuf"; }));
  unitTest.test("FIND", pBibouDir->find([](std::shared_ptr<Object> o) { return o->name() == "tata"; }) != nullptr);
  unitTest.test("FIND2", pBibouDir->find([](std::shared_ptr<Object> o) { return o->name() == "teufteuf"; }) == nullptr);
  unitTest.test("HAS_CHILD", root->hasChild("bibou"));
  unitTest.test("MKDIR", root->mkdir("bibou") == pBibouDir);
  unitTest.test("PARENT_IS_ROOT", pBibouDir->parent() == root);

  return 0;
}
