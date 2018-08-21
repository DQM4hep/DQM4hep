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
#include <dqm4hep/Logging.h>
#include <dqm4hep/Plugin.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/Version.h>

namespace dqm4hep {

  namespace core {

    Plugin::Plugin(const std::string &name) : 
      m_pluginName(name) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    Plugin::~Plugin() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Plugin::pluginName() const {
      return m_pluginName;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode Plugin::registerMe() {
      return PluginManager::instance()->registerPlugin(this);
    }
  }
}
