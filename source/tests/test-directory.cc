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

#include "dqm4hep/Directory.h"

// -- std headers
#include <iostream>
#include <signal.h>

using namespace std;
using namespace dqm4hep::core;

#define assert_test(Command) \
  if( ! (Command) )\
  {\
    dqm_error( "Assertion failed : {0}", #Command );\
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

int main(int argc, char* argv[])
{
  Logger::createLogger("test-directory", {Logger::coloredConsole()});
  Logger::setMainLogger("test-directory");
  Logger::setLogLevel(spdlog::level::debug);

  Directory_t root("root");
  Directory_t *heroes = root.mkdir("heroes");

  Object *pObject = nullptr;
  pObject = new Object("superman"); heroes->add(pObject);
  pObject = new Object("spiderman"); heroes->add(pObject);
  pObject = new Object("batman"); heroes->add(pObject);
  assert_test(heroes->contents().size() == 3);

  heroes->remove(pObject);
  assert_test(heroes->contents().size() == 2);

  root.clear();
  assert_test(root.isEmpty());

  Directory_t *pBibouDir = root.mkdir("bibou");
  pBibouDir->add(new Object("toto"));
  pBibouDir->add(new Object("tata"));
  pBibouDir->add(new Object("tutu"));
  assert_test(pBibouDir->contents().size() == 3);

  pBibouDir->remove( [](Object *pObject){ return pObject->name() == "toto"; } );
  assert_test(pBibouDir->contents().size() == 2);
  Path bibouPath = pBibouDir->fullPath();
  assert_test(bibouPath.getPath() == "/root/bibou");
  assert_test(!pBibouDir->isRoot());
  assert_test(root.isRoot());
  assert_test(!root.isEmpty());
  assert_test(!pBibouDir->isEmpty());
  assert_test(pBibouDir->contains( [](Object *pObject){ return pObject->name() == "tata"; } ));
  assert_test(!pBibouDir->contains( [](Object *pObject){ return pObject->name() == "teufteuf"; } ));
  assert_test(pBibouDir->find( [](Object *pObject){ return pObject->name() == "tata"; } ) != nullptr);
  assert_test(pBibouDir->find( [](Object *pObject){ return pObject->name() == "teufteuf"; } ) == nullptr);
  assert_test(root.hasChild("bibou"));
  assert_test(root.mkdir("bibou") == pBibouDir);
  assert_test(pBibouDir->parent() == &root);

  return 0;
}
