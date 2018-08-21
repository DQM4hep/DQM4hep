//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dqm4hep/AppTimer.h"
#include "dqm4hep/AppEventLoop.h"

namespace dqm4hep {

  namespace online {
    
    AppTimer::AppTimer(AppEventLoop &loop) :
      m_eventLoop(loop) {
    }
    
    //-------------------------------------------------------------------------------------------------
    
    AppTimer::~AppTimer() {
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppTimer::setSingleShot(bool single) {
      m_singleShot = single;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool AppTimer::singleShot() const {
      return m_singleShot.load();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppTimer::setInterval(unsigned int msec) {
      m_interval = msec;
      m_eventLoop.startTimer(this);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    unsigned int AppTimer::interval() const {
      return m_interval.load();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::Signal<> &AppTimer::onTimeout() {
      return m_signal;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppTimer::start() {
      m_eventLoop.startTimer(this);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppTimer::stop() {
      m_eventLoop.stopTimer(this);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool AppTimer::active() const {
      return m_active.load();
    }

  }

} 

