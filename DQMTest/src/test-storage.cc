/// \file test-storage.cc
/*
 *
 * test-storage.cc main source file template automatically generated
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
