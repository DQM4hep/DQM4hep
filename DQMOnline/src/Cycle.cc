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
#include "dqm4hep/Cycle.h"
#include "dqm4hep/AppEvents.h"

namespace dqm4hep {

  namespace online {
    
    std::ostream& operator<<(std::ostream &out, const EOCCondition &condition) {
      out << "time: " << condition.m_totalTime << " sec, "
          << "counter: " << condition.m_counter << ", "
          << "timeout? " << (condition.m_timeoutReached ? "true" : "false") << ", "
          << "forced? " << (condition.m_forcedEnd ? "true" : "false");
      return out;
    }

    //-------------------------------------------------------------------------------------------------

    Cycle::Cycle(AppEventLoop &loop) :
      m_eventLoop(loop) {
      m_thread = std::thread(&Cycle::cycleLoop, this);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    Cycle::~Cycle() {
      m_stopFlag = true;
      m_thread.join();
    }
    
    //-------------------------------------------------------------------------------------------------

    unsigned int Cycle::timeout() const {
      return m_timeout.load();
    }
    
    //-------------------------------------------------------------------------------------------------

    void Cycle::setTimeout(unsigned int value) {
      m_timeout = value;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    unsigned int Cycle::timerPeriod() const {
      return m_period.load();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void Cycle::setTimerPeriod(unsigned int value) {
      m_period = value;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    unsigned int Cycle::counterLimit() const {
      return m_counterLimit.load();
    }
    
    //-------------------------------------------------------------------------------------------------

    void Cycle::setCounterLimit(unsigned int value) {
      m_counterLimit = value;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void Cycle::setEventPriority(int priority) {
      if(priority >= 0 and priority <= 100) {
        m_eventPriority = priority;
      }
    }
    
    //-------------------------------------------------------------------------------------------------

    int Cycle::eventPriority() const {
      return m_eventPriority.load();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void Cycle::incrementCounter(unsigned int increment) {
      m_currentCounter += increment;
      m_lastCounterIncrement = core::time::now();
    }
    
    //-------------------------------------------------------------------------------------------------

    void Cycle::startCycle(bool emit) {
      if(m_timeout >= m_period || (0 == m_counterLimit && 0 == m_period)) {
        dqm_error("Cycle::startCycle: invalid cycle settings (timeout {0}, counter limit {1}, period {2}) !", m_timeout.load(), m_counterLimit.load(), m_period.load());
        throw core::StatusCodeException(core::STATUS_CODE_FAILURE);
      }
      forceStopCycle(true, emit);
      m_startCycleFlag = true;
      dqm_debug( "=====> Started new cycle !" );
    }
    
    //-------------------------------------------------------------------------------------------------

    void Cycle::forceStopCycle(bool waitEnd, bool emit) {
      if(running()) {
        m_eocEmit = emit;
        m_forceStopFlag = true;
        if(waitEnd) {
          while(running()) {
            usleep(1000);
          }          
        }
      }
    }
    
    //-------------------------------------------------------------------------------------------------

    bool Cycle::running() const {
      return m_running.load();
    }
    
    //-------------------------------------------------------------------------------------------------

    void Cycle::cycleLoop() {
      while(1) {
        if(m_stopFlag.load()) {
          break;          
        }
        if(not m_startCycleFlag.load()) {
          usleep(10000);
          continue;
        }
        // start of cycle here
        m_running = true;
        m_forceStopFlag = false;
        m_eocEmit = true;
        EOCCondition condition;
        condition.m_startTime = core::time::now();
        condition.m_forcedEnd = false;
        m_lastCounterIncrement = condition.m_startTime;
        m_currentCounter = 0;
        while(1) {
          if(m_stopFlag.load() or m_forceStopFlag.load()) {
            condition.m_forcedEnd = true;
            if(m_eocEmit.load()) {
              endOfCycle(condition);
              break;
            }
          }
          const core::time::point now = core::time::now();
          const float timeoutEllapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now-m_lastCounterIncrement).count() / 1000.f;
          const bool timeoutReached = (0 == m_timeout) ? false : (timeoutEllapsed >= m_timeout.load());
          // check timeout first
          if(timeoutReached) {
            condition.m_timeoutReached = true;
            endOfCycle(condition);
            break;
          }
          // check for normal termination of cycle
          const bool counterOver = (0 == m_counterLimit) ? false : (m_currentCounter >= m_counterLimit);
          const float ellapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now-condition.m_startTime).count() / 1000.f;
          const bool periodReached = (0 == m_period) ? false : (ellapsed >= m_period.load());
          if(counterOver or periodReached) {
            endOfCycle(condition);
            break;
          }
          usleep(10000);
        }
        // reset cycle properties
        m_startCycleFlag = false;
        m_forceStopFlag = false;
        m_eocEmit = true;
        m_running = false;
        usleep(10000);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void Cycle::endOfCycle(EOCCondition &condition) {
      condition.m_endTime = core::time::now();
      condition.m_counter = m_currentCounter.load();
      condition.m_totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(condition.m_endTime - condition.m_startTime).count() / 1000.f;
      condition.m_rate = (condition.m_startTime == condition.m_endTime) ? 0 : (condition.m_counter / condition.m_totalTime);
      dqm_debug( "End of cycle !" );
      // post event to event loop
      auto *event = new StoreEvent<EOCCondition>(AppEvent::END_OF_CYCLE, condition);
      event->setPriority(m_eventPriority.load());
      m_eventLoop.postEvent(event);
    }

  }

}

