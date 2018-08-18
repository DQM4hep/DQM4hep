/// \file test-cycle.cc
/*
 *
 * test-cycle.cc main source file template automatically generated
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
#include <dqm4hep/Cycle.h>
#include <dqm4hep/Application.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace dqm4hep::core;
using namespace dqm4hep::online;

#define assert_test(Command)                                                                                           \
  if (!(Command)) {                                                                                                    \
    dqm_error("Assertion failed : {0}, line {1}", #Command, __LINE__);                                                 \
    ::exit(1);                                                                                                           \
  }

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class TestApplication : public Application {
public:
  ~TestApplication() {}
  virtual void parseCmdLine(int /*argc*/, char ** /*argv*/) override {}
  virtual void onInit() override {}
  virtual void onStop() override {}
  virtual void onEvent(AppEvent *pAppEvent) override = 0;
  
  TestApplication(const std::string &appName, unsigned int period, unsigned int timeout, unsigned int counterLimit) :
    Application(),
    m_cycle(m_eventLoop) {
    m_cycle.setTimerPeriod(period);
    m_cycle.setTimeout(timeout);
    m_cycle.setCounterLimit(counterLimit);
    setType("test");
    setName(appName);
    setLogLevel(spdlog::level::debug);
    enableStats(false);
    setNoServer(true);
  }

  virtual void onStart() override {
    m_cycle.startCycle();
  }
  
protected:
  Cycle             m_cycle;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class TimeoutTestApp : public TestApplication {
public:
  TimeoutTestApp() :
    TestApplication("timeout", 3, 2, 10) {  
    setNoServer(true);
  }
  
  void onEvent(AppEvent *pAppEvent) override {
    auto* eocEvent = dynamic_cast<StoreEvent<EOCCondition>*>(pAppEvent);
    if(eocEvent) {
      dqm_debug("Got an end of cycle: {0}", typeToString(eocEvent->data()));
      assert_test(eocEvent->data().m_timeoutReached);
      assert_test(not eocEvent->data().m_forcedEnd);
      m_nCycles++;
      if(m_nCycles == 3) {
        exit(0);
      }
      else {
        m_cycle.startCycle();
      }
    }
  }  
public:
  unsigned int      m_nCycles = {0};
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class NormalTestApp : public TestApplication {
public:
  NormalTestApp() :
    TestApplication("normal", 2, 0, 10) {  
    setNoServer(true);
  }
  
  void onEvent(AppEvent *pAppEvent) override {
    auto* eocEvent = dynamic_cast<StoreEvent<EOCCondition>*>(pAppEvent);
    if(eocEvent) {
      dqm_debug("Got an end of cycle: {0}", typeToString(eocEvent->data()));
      assert_test(not eocEvent->data().m_timeoutReached);
      assert_test(not eocEvent->data().m_forcedEnd);
      m_nCycles++;
      if(m_nCycles == 3) {
        exit(0);
      }
      else {
        m_cycle.startCycle();
      }
    }
  }  
public:
  unsigned int      m_nCycles = {0};
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class NoTimeoutTestApp : public TestApplication {
public:
  NoTimeoutTestApp() :
    TestApplication("notimeout", 3, 2, 10) { 
    setNoServer(true);
  }
  NoTimeoutTestApp(const NoTimeoutTestApp&) = delete;
  NoTimeoutTestApp& operator=(const NoTimeoutTestApp&) = delete;
  ~NoTimeoutTestApp() {
    removeTimer(m_incrementTimer);
  }
  
  void increment() {
    dqm_debug("Increment cycle value ++ !!");
    m_cycle.incrementCounter();
  }
  
  void onInit() override {
    m_incrementTimer = createTimer();
    m_incrementTimer->setInterval(1000);
    m_incrementTimer->setSingleShot(false);
    m_incrementTimer->onTimeout().connect(this, &NoTimeoutTestApp::increment);
    m_incrementTimer->start();
  }
  
  void onEvent(AppEvent *pAppEvent) override {
    auto* eocEvent = dynamic_cast<StoreEvent<EOCCondition>*>(pAppEvent);
    if(eocEvent) {
      dqm_debug("Got an end of cycle: {0}", typeToString(eocEvent->data()));
      assert_test(not eocEvent->data().m_timeoutReached);
      assert_test(not eocEvent->data().m_forcedEnd);
      assert_test(eocEvent->data().m_counter == 3);
      m_nCycles++;
      if(m_nCycles == 3) {
        m_incrementTimer->stop();
        exit(0);
      }
      else {
        m_cycle.startCycle();
      }
    }
  }  
public:
  AppTimer*         m_incrementTimer = {nullptr};
  unsigned int      m_nCycles = {0};
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

int main(int argc, char **argv) {
  TimeoutTestApp timeoutApp;
  timeoutApp.init(argc, argv);
  timeoutApp.exec();
  assert_test(timeoutApp.m_nCycles == 3);
  
  NormalTestApp normalApp;
  normalApp.init(argc, argv);
  normalApp.exec();
  assert_test(normalApp.m_nCycles == 3);
  
  NoTimeoutTestApp notimeoutApp;
  notimeoutApp.init(argc, argv);
  notimeoutApp.exec();
  assert_test(notimeoutApp.m_nCycles == 3);
  
  return 0;
}

