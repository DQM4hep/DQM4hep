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
#include <dqm4hep/Handle.h>

namespace dqm4hep {

  namespace core {

    void *Handle::raw() {
      return m_ptr.get();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const void *Handle::raw() const {
      return m_ptr.get();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::type_index &Handle::typeInfo() const {
      return m_typeInfo;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool Handle::isValid() const {
      return (nullptr != raw());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    Handle Handle::create() const {
      return m_newFunction();
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    TClass *ROOTHandle::getClass() const {
      return m_class;
    }

  }
  
}
