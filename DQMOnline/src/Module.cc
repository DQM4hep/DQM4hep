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
#include "dqm4hep/Module.h"

namespace dqm4hep {

  namespace online {

    void Module::setName(const std::string &module) {
      m_name = module;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Module::name() const {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    void Module::setDetectorName(const std::string &detector) {
      m_detectorName = detector;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string &Module::detectorName() const {
      return m_detectorName;
    }

    //-------------------------------------------------------------------------------------------------

    void Module::setVersion(unsigned int major, unsigned int minor, unsigned int patch) {
      m_version.set(major, minor, patch);
    }
    
    //-------------------------------------------------------------------------------------------------

    const core::Version &Module::version() const {
      return m_version;
    }
    
    //-------------------------------------------------------------------------------------------------

    ModuleApplication *Module::moduleApplication() const {
      return m_moduleApplication;
    }

    //-------------------------------------------------------------------------------------------------

    void Module::setModuleApplication(ModuleApplication *application) {
      m_moduleApplication = application;
    }

  }

}

