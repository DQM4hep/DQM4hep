//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_PLUGIN_H
#define DQM4HEP_PLUGIN_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>

namespace dqm4hep {

  namespace core {

    class PluginManager;

    /** Plugin class
     */
    class Plugin {
      friend class PluginManager;

    public:
      /** Constructor
       */
      Plugin(const std::string &pluginName);

      /** Destructor
       */
      virtual ~Plugin();

      /** Returns the plug-in name
       */
      const std::string &pluginName() const;

      /** Create a new instance of the wrapped class
       */
      virtual Plugin *create() const = 0;

      /** Returns the class name
       */
      virtual std::string className() const = 0;

    protected:
      StatusCode registerMe();

    private:
      const std::string m_pluginName;
    };
  }
}

#endif //  DQM4HEP_PLUGIN_H
