//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_PLUGINMANAGER_H
#define DQM4HEP_PLUGINMANAGER_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/Plugin.h>
#include <dqm4hep/Singleton.h>
#include <dqm4hep/StatusCodes.h>

//-------------------------------------------------------------------------------------------------

#define DQM_PLUGIN_DECL(ClassName, ClassStr)                                                                           \
  class DQMPlugin_##ClassName : public dqm4hep::core::Plugin, public ClassName {                                       \
  public:                                                                                                              \
    DQMPlugin_##ClassName(bool reg) : dqm4hep::core::Plugin(ClassStr), ClassName() {                                   \
      if (reg)                                                                                                         \
        THROW_RESULT_IF(dqm4hep::core::STATUS_CODE_SUCCESS, !=, this->registerMe());                                   \
    }                                                                                                                  \
    ~DQMPlugin_##ClassName() = default;                                                                                \
    dqm4hep::core::Plugin *create() const override {                                                                            \
      return new DQMPlugin_##ClassName(false);                                                                         \
    }                                                                                                                  \
    std::string className() const override {                                                                                    \
      return std::string(#ClassName);                                                                                  \
    }                                                                                                                  \
  };                                                                                                                   \
  static DQMPlugin_##ClassName instance_DQMPlugin_##ClassName(true)

//-------------------------------------------------------------------------------------------------

namespace dqm4hep {

  namespace core {

    class Plugin;

    /** PluginManager class.
     *  Responsible for loading shared libraries
     *  that contains Plugin instances
     */
    class PluginManager : public Singleton<PluginManager> {
      friend class Singleton<PluginManager>;
      friend class Plugin;

      typedef std::map<const std::string, Plugin *> PluginMap;

    public:
      /** Load shared libraries from the environment variable 4HEP_PLUGIN_DLL
       */
      StatusCode loadLibraries();

      /** Load the shared libraries
       */
      StatusCode loadLibraries(const StringVector &libraryNameList);

      /** Load the shared library
       */
      StatusCode loadLibrary(const std::string &libraryName);

      /** Get the plug-in by name
       */
      const Plugin *getPlugin(const std::string &pluginName) const;

      /** Get the plug-in clone. A new plug-in instance is allocated and returned
       *  to the user. Ownership of the plug-in transfered to the caller.
       */
      template <typename T>
      std::shared_ptr<T> create(const std::string &pluginName) const;

      /** Whether the plug-in is registered within the plug-in manager
       */
      bool isPluginRegistered(const std::string &pluginName) const;

      /** Get the plug-in name list
       */
      StringVector pluginNames() const;

      /** Get the plugin name list matching the type T
       */
      template <typename T>
      StringVector pluginNamesMatchingType() const;

      /** Dump registered plugin via logging
       */
      void dump() const;

    private:
      /** Default constructor
       */
      PluginManager() = default;

      /** Destructor
       */
      ~PluginManager();

      /** Register the plug-in. Only available by plug-ins themselves
       */
      StatusCode registerPlugin(Plugin *pPlugin);

    private:
      PluginMap               m_pluginMap = {};       ///< The map of registered plugins
      std::vector<void*>      m_dlLibraries = {};     ///< The list of loaded libraries
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline std::shared_ptr<T> PluginManager::create(const std::string &pluginName) const {
      const Plugin *pPlugin = this->getPlugin(pluginName);

      if (!pPlugin)
        return std::shared_ptr<T>();

      Plugin *pClass = pPlugin->create();

      if (nullptr == pClass)
        return std::shared_ptr<T>();

      return std::shared_ptr<T>(dynamic_cast<T *>(pClass));
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StringVector PluginManager::pluginNamesMatchingType() const {
      StringVector names;

      for (auto plugin : m_pluginMap) {
        const Plugin *pPlugin(plugin.second);

        if (nullptr != dynamic_cast<const T *>(pPlugin))
          names.push_back(plugin.first);
      }

      return names;
    }
  }
}

#endif //  DQM4HEP_PLUGINMANAGER_H
