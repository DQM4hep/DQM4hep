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
#include <dqm4hep/Cycle.h>
#include <dqm4hep/Application.h>
#include <dqm4hep/UnitTesting.h>

// -- std headers
#include <iostream>
#include <signal.h>

using namespace dqm4hep::core;
using namespace dqm4hep::online;
using UnitTest = dqm4hep::test::UnitTest;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class TestApplication : public Application {
public:
  ~TestApplication() {}
  virtual void parseCmdLine(int /*argc*/, char ** /*argv*/) override {}
  virtual void onInit() override {}
  virtual void onStop() override {}
  virtual void onEvent(AppEvent *pAppEvent) override = 0;
  
  TestApplication(UnitTest& unitTest, const std::string &appName, unsigned int period, unsigned int timeout, unsigned int counterLimit) :
    Application(),
    m_cycle(m_eventLoop),
    m_unitTest(unitTest) {
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
  UnitTest&         m_unitTest;
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

class TimeoutTestApp : public TestApplication {
public:
  TimeoutTestApp(UnitTest &test) :
    TestApplication(test, "timeout", 3, 2, 10) {  
    setNoServer(true);
  }
  
  void onEvent(AppEvent *pAppEvent) override {
    auto* eocEvent = dynamic_cast<StoreEvent<EOCCondition>*>(pAppEvent);
    if(eocEvent) {
      dqm_debug("Got an end of cycle: {0}", typeToString(eocEvent->data()));
      m_unitTest.test("CHECK_TIMEOUT", eocEvent->data().m_timeoutReached);
      m_unitTest.test("EOC_NOT_FORCED", not eocEvent->data().m_forcedEnd);
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
  NormalTestApp(UnitTest &test) :
    TestApplication(test, "normal", 2, 0, 10) {  
    setNoServer(true);
  }
  
  void onEvent(AppEvent *pAppEvent) override {
    auto* eocEvent = dynamic_cast<StoreEvent<EOCCondition>*>(pAppEvent);
    if(eocEvent) {
      dqm_debug("Got an end of cycle: {0}", typeToString(eocEvent->data()));
      m_unitTest.test("CHECK_TIMEOUT2", not eocEvent->data().m_timeoutReached);
      m_unitTest.test("EOC_NOT_FORCED2", not eocEvent->data().m_forcedEnd);
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
  NoTimeoutTestApp(UnitTest &test) :
    TestApplication(test, "notimeout", 3, 2, 10) { 
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
      m_unitTest.test("CHECK_TIMEOUT3", not eocEvent->data().m_timeoutReached);
      m_unitTest.test("EOC_NOT_FORCED3", not eocEvent->data().m_forcedEnd);
      m_unitTest.test("CHECK_COUNTER", eocEvent->data().m_counter == 3);
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
  
  UnitTest unitTest("test-cycle");
  
  TimeoutTestApp timeoutApp(unitTest);
  timeoutApp.init(argc, argv);
  timeoutApp.exec();
  unitTest.test("CHECK_COUNTER2", timeoutApp.m_nCycles == 3);
  
  NormalTestApp normalApp(unitTest);
  normalApp.init(argc, argv);
  normalApp.exec();
  unitTest.test("CHECK_COUNTER3", normalApp.m_nCycles == 3);
  
  NoTimeoutTestApp notimeoutApp(unitTest);
  notimeoutApp.init(argc, argv);
  notimeoutApp.exec();
  unitTest.test("CHECK_COUNTER4", notimeoutApp.m_nCycles == 3);
  
  return 0;
}

