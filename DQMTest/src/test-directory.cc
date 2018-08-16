/// \file test-directory.cc
/*
 *
 * test-directory.cc main source file template automatically generated
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
