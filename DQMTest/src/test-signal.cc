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
#include <dqm4hep/UnitTesting.h>

using namespace std;
using namespace dqm4hep::core;
using UnitTest = dqm4hep::test::UnitTest;

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
  UnitTest unitTest("test-signal");

  TestSignal test;

  Signal<> sig1;
  unitTest.test("CONNECT1", sig1.connect(func1)); // global function
  unitTest.test("CONNECT2", sig1.connect(&test, &TestSignal::slot)); // class member-function
  unitTest.test("HAS_CONNECTION1", sig1.hasConnection());
  unitTest.test("N_CONNECTION1", sig1.nConnections() == 2);
  unitTest.test("HAS_CONNECTION2", sig1.hasConnection(&test));
  unitTest.test("IS_CONNECTED1", sig1.isConnected(func1));
  unitTest.test("IS_NOT_CONNECTED1", not sig1.isConnected(&test, &TestSignal::otherSlot));
  
  counter = 0;
  sig1.emit();
  unitTest.test("EMIT_COUNTER1", counter == 2);
  sig1.emit();
  unitTest.test("EMIT_COUNTER2", counter == 4);
  
  unitTest.test("DISCONNECT1", sig1.disconnect(func1));
  unitTest.test("IS_NOT_CONNECTED2", not sig1.isConnected(func1));
  sig1.emit();
  unitTest.test("N_CONNECTION2", sig1.nConnections() == 1);
  unitTest.test("EMIT_COUNTER3", counter == 5);
  
  unitTest.test("CONNECT3", sig1.connect(func1)); // connect again
  unitTest.test("N_CONNECTION3", sig1.nConnections() == 2);
  unitTest.test("DISCONNECT2", sig1.disconnect(&test, &TestSignal::slot));
  unitTest.test("N_CONNECTION4", sig1.nConnections() == 1);
  sig1.emit();
  unitTest.test("EMIT_COUNTER4", counter == 6);
  
  return 0;
}
