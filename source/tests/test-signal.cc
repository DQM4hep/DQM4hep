/// \file test-signal.cc
/*
 *
 * test-signal.cc main source file template automatically generated
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
#include <dqm4hep/Signal.h>

using namespace std;
using namespace dqm4hep::core;

#define assert_test(Command)                                                                                           \
  if (!(Command)) {                                                                                                    \
    dqm_error("Assertion failed : {0}, line {1}", #Command, __LINE__);                                                 \
    exit(1);                                                                                                           \
  }

unsigned int counter = 0;

void func1() {
  counter++;
  dqm_info( "func1, counter is now {0}", counter );
}

void func2(int &value) {
  int old = value;
  value++;
  dqm_info( "func2(), value was {0} and is now {1}", old, value );
}

class TestSignal {
public:
  void slot() {
    counter++;
    dqm_info( "TestSignal::slot(), counter is now {0}", counter );
  }
  
  void otherSlot() {
    
  }
  
  void increment(int &value) {
    int old = value;
    value++;
    dqm_info( "TestSignal::increment(), value was {0} and is now {1}", old, value );
  }
};

int main(int /*argc*/, char ** /*argv*/) {
  Logger::createLogger("test-signal", {Logger::coloredConsole()});
  Logger::setMainLogger("test-signal");
  Logger::setLogLevel(spdlog::level::debug);

  TestSignal test;

  Signal<> sig1;
  assert_test(sig1.connect(func1)); // global function
  assert_test(sig1.connect(&test, &TestSignal::slot)); // class member-function
  assert_test(sig1.hasConnection());
  assert_test(sig1.nConnections() == 2);
  assert_test(sig1.hasConnection(&test));
  assert_test(sig1.isConnected(func1));
  assert_test(not sig1.isConnected(&test, &TestSignal::otherSlot));
  
  counter = 0;
  sig1.emit();
  assert_test(counter == 2);
  sig1.emit();
  assert_test(counter == 4);
  
  assert_test(sig1.disconnect(func1));
  assert_test(not sig1.isConnected(func1));
  sig1.emit();
  assert_test(sig1.nConnections() == 1);
  assert_test(counter == 5);
  
  assert_test(sig1.connect(func1)); // connect again
  assert_test(sig1.nConnections() == 2);
  assert_test(sig1.disconnect(&test, &TestSignal::slot));
  assert_test(sig1.nConnections() == 1);
  sig1.emit();
  assert_test(counter == 6);
  
  return 0;
}
