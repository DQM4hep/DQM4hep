//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_CYCLE_H
#define DQM4HEP_CYCLE_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/AppEventLoop.h"

// -- std headers
#include <atomic>

namespace dqm4hep {

  namespace online {
    
    /**
     *  @brief  EOCCondition struct
     *          The end of cycle conditions
     */
    struct EOCCondition {
      /// The start time of the cycle
      core::time::point   m_startTime = {};
      /// The end time of the cycle
      core::time::point   m_endTime = {};
      /// The total cycle time (unit seconds)
      float               m_totalTime = {0.f};
      /// The counter value when the cycle ended
      unsigned int        m_counter = {0};
      /// The mean processing rate: counter/total time (unit n/sec) 
      float               m_rate = {0.f};
      /// Whether the end of cycle was caused by a timeout
      bool                m_timeoutReached = {false};
      /// Whether the end of cycle was forced by an external condition
      bool                m_forcedEnd = {false};
    };
    
    std::ostream& operator<<(std::ostream &out, const EOCCondition &condition);
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
        
    /** Cycle class.
     *
     *  Process a cycle of processEvent(evt) call for a AnalysisModule.
     *  See implementation for cycle types.
     *
     *  The cycle value is the value for which the cycle should normally ends.
     *
     *  The timeout value is the maximum time between two processEvent(evt) call
     *  When the timeout is reached, the cycle ends. The default timeout value
     *  is 10 seconds and can be changed via the method setTimeout(secs)
     */
    class Cycle {
    public:      
      /** 
       *  @brief  Constructor
       *
       *  @param  loop the application event loop
       */
      Cycle(AppEventLoop &loop);

      /** 
       *  @brief  Destructor
       */
      ~Cycle();
      
      /**
       *  @brief  Get the timeout value (unit seconds)
       */
      unsigned int timeout() const;
      
      /**
       *  @brief  Set the cycle timeout value (unit seconds)
       *  
       *  @param value the timeout value in seconds
       */
      void setTimeout(unsigned int value);
      
      /**
       *  @brief  Get the timer period value (unit seconds)
       */
      unsigned int timerPeriod() const;
      
      /**
       *  @brief  Set the cycle timer period value (unit seconds)
       *  
       *  @param value the period in seconds
       */
      void setTimerPeriod(unsigned int value);

      /**
       *  @brief  Set the couting limit
       * 
       *  @param value the couting limit
       */
      void setCounterLimit(unsigned int value);
      
      /**
       *  @brief  Get the couting limit
       */
      unsigned int counterLimit() const;
      
      /**
       *  @brief  Set the EndOfCycleEvent event priority
       * 
       *  @param  priority the EndOfCycleEvent priority
       */
      void setEventPriority(int priority);
      
      /**
       *  @brief  Get the EndOfCycleEvent event priority
       */
      int eventPriority() const;
      
      /**
       *  @brief  Increment the counter by a value. Default is 1
       *  
       *  @param increment the increment value
       */
      void incrementCounter(unsigned int increment = 1);

      /** 
       *  @brief  Start a new cycle
       *          Stop the current cycle if running.
       *          User can choose whether to emit the end of cyle signal
       *
       *  @param  emit whether to emit the end of cycle signal
       */
      void startCycle(bool emit = false);

      /** 
       *  @brief  Stop the current cycle if running.
       *          User can choose whether to emit the end of cyle signal
       *          and wait for the end of cycle
       *
       *  @param  waitEnd whether to wait for status "not running"
       *  @param  emit whether to emit the end of cycle signal
       */
      void forceStopCycle(bool waitEnd = true, bool emit = false);
      
      /** 
       *  @brief  Whether the cycle is running
       */
      bool running() const;
      
    private:
      /**
       *  @brief  The main cycle loop thread
       */
      void cycleLoop();
      
      /**
       *  @brief  Called at end of cycle. 
       *          Fill the end of cycle conditions and post an event in the event loop
       *  
       *  @param condition the condition to fill and post
       */
      void endOfCycle(EOCCondition &condition);

    private:
      /// The application event loop in which the cycle is running
      AppEventLoop&                    m_eventLoop;
      /// Whether a cycle has been started 
      std::atomic_bool                 m_running = {false};
      /// An internal flag to trigger a new cycle
      std::atomic_bool                 m_startCycleFlag = {false};
      /// An internal flag to trigger an end of cycle
      std::atomic_bool                 m_forceStopFlag = {false};
      /// Whether an event has to be posted on end of cycle
      std::atomic_bool                 m_eocEmit = {true};
      /// An internal flag to stop the cycle thread
      std::atomic_bool                 m_stopFlag = {false};
      /// The thread object running the cycle function
      std::thread                      m_thread = {};
      /// The user timeout value, default 10 (unit seconds)
      std::atomic_uint                 m_timeout = {10};
      /// The user timer period, default 30 (unit seconds)
      std::atomic_uint                 m_period = {30};
      /// The user counter limit
      std::atomic_uint                 m_counterLimit = {0};
      /// The internal counter incremented by user 
      std::atomic_uint                 m_currentCounter = {0};
      /// The end of cycle event priority in the event loop 
      std::atomic_int                  m_eventPriority = {60};
      /// The last time point when the increement method was called
      core::time::point                m_lastCounterIncrement = {};
    };

  }

} 

#endif  //  DQM4HEP_CYCLE_H
