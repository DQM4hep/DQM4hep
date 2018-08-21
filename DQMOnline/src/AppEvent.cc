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
#include "dqm4hep/AppEvent.h"

namespace dqm4hep {

  namespace online {
    
    AppEvent::AppEvent(int t) :
      m_type(t) {
        /* nop */
    }
    
    //-------------------------------------------------------------------------------------------------
    
    AppEvent::~AppEvent() {
      /* nop */
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int AppEvent::type() const {
      return m_type;
    }
    
    //-------------------------------------------------------------------------------------------------
        
    void AppEvent::setPriority(int p) {
      if(p > 100) {
        m_priority = 100;
      }
      else if(p < 0) {
        m_priority = 0;
      }
      else {
        m_priority = p;
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int AppEvent::priority() const {
      return m_priority;
    }

  }

}

