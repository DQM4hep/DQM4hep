//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_APPTIMER_H
#define DQM4HEP_APPTIMER_H

#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/Signal.h"

#include <atomic>

namespace dqm4hep {
  
  namespace online {
    
    class AppEventLoop;

    /**
     *  @brief  AppTimer class
     */
    class AppTimer {
      friend class AppEventLoop;
    public:
      /**
       *  @brief  Constructor
       */
      AppTimer(AppEventLoop &loop);
      
      /**
       *  @brief  Destructor
       */
      ~AppTimer();
      
      /**
       *  @brief  Set whether the timer is a single shot timer.
       *          After the timer reaches the timeout, if the 
       *          timer is not single shot, it is automatically restarted
       *          
       *  @param  single whether the timer is single shot 
       */
      void setSingleShot(bool single);
      
      /**
       *  @brief  Whether the timer is single shot
       */
      bool singleShot() const;
        
      /**
       *  @brief  Set the timer interval (unit milliseconds)
       * 
       *  @param  msec the timer interval (unit milliseconds)
       */
      void setInterval(unsigned int msec);
      
      /**
       *  @brief  Get the timer interval (unit milliseconds)
       */
      unsigned int interval() const;
      
      /**
       *  @brief  Get the timeout signal
       */
      core::Signal<> &onTimeout();
      
      /**
       *  @brief  Start the timer
       */
      void start();
      
      /**
       *  @brief  Stop the timer
       */
      void stop();
      
      /**
       *  @brief  Whether the timer is active
       */
      bool active() const;
      
    private:
      /// The application event loop
      AppEventLoop&                m_eventLoop;
      /// Whether the timer is single shot
      std::atomic_bool             m_singleShot = {true};
      /// The timer interval (unit milliseconds)
      std::atomic_uint             m_interval = {0};
      /// The timer timeout signal
      core::Signal<>               m_signal = {};
      /// Whether the timer is active
      std::atomic_bool             m_active = {false};
      /// The time point when the timer was started
      core::TimePoint              m_startTime = {};
    };
  
  }

} 

#endif  //  DQM4HEP_APPTIMER_H
