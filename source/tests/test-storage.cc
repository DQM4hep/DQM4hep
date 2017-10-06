/// \file test-monitor-elements.cc
/*
 *
 * test-monitor-elements.cc main source file template automatically generated
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
#include "dqm4hep/Storage.h"

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

class Object
{
public:
  Object(const std::string &name) :
    m_name(name)
    {}
  const std::string &name() {return m_name;}
private:
  std::string    m_name;
};

typedef Directory<Object> Directory_t;
typedef Storage<Object> Storage_t;
typedef std::vector<Object*> ObjectList;

int main(int argc, char* argv[])
{
  Logger::createLogger("test-storage", {Logger::coloredConsole()});
  Logger::setMainLogger("test-storage");
  Logger::setLogLevel(spdlog::level::debug);

  Storage_t storage;

  assert_test(storage.mkdir("/heroes/best") == STATUS_CODE_SUCCESS);
  assert_test(storage.mkdir("/heroes/worst") == STATUS_CODE_SUCCESS);

  ObjectList list;
  storage.getObjects(list);
  assert_test(list.size() == 0);

  Object *pObject = nullptr;

  assert_test(storage.cd("/heroes/worst") == STATUS_CODE_SUCCESS);
  pObject = new Object("Batman"); storage.add(pObject);
  pObject = new Object("SuperLezard"); storage.add(pObject);
  pObject = new Object("ConcombreMan"); storage.add(pObject);

  storage.getObjects(list);
  assert_test(list.size() == 3);
  list.clear();

  assert_test(storage.pwd() == "worst");

  assert_test(storage.cd("/heroes/best") == STATUS_CODE_SUCCESS);
  pObject = new Object("Superman"); storage.add(pObject);
  pObject = new Object("Spiderman"); storage.add(pObject);
  pObject = new Object("Me"); storage.add(pObject);

  storage.getObjects(list);
  assert_test(list.size() == 6);

  assert_test(storage.dirExists("/heroes/best"));
  assert_test(!storage.dirExists("best"));
  assert_test(storage.dirExists("/heroes"));

  assert_test(storage.goUp() == STATUS_CODE_SUCCESS);
  assert_test(storage.pwd() == "heroes");

  assert_test(storage.rmdir("/heroes/worst") == STATUS_CODE_SUCCESS);

  return 0;
}
