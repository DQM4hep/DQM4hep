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
#include <dqm4hep/StatusCodes.h>

// -- std headers
#include <typeindex>

//-------------------------------------------------------------------------------------------------

#define DQM_PLUGIN_DECL(ClassName, ClassStr) DQM4HEP_DECLARE_PLUGIN(ClassName, ClassStr)
    
#define DQM4HEP_DECLARE_PLUGIN(ClassName, id) \
  static const dqm4hep::core::Plugin static_plugin_instance_##ClassName{id, ClassName{}}

//-------------------------------------------------------------------------------------------------

namespace dqm4hep {

  namespace core {

    class Plugin;

    /** 
     *  @brief  PluginManager class.
     *  Responsible for loading shared libraries
     *  that contains Plugin instances
     */
    class PluginManager {
      friend class Plugin;
      typedef std::map<const std::string, const Plugin *> PluginMap;

    public:
      /** Return a unique instance of the class
       */
      static PluginManager *instance();

      /** Kill the unique instance of the class
       */
      static void kill();
      
      /** 
       *  @brief  Load shared libraries from the environment variable DQM4HEP_PLUGIN_DLL
       */
      StatusCode loadLibraries();

      /** 
       *  @brief  Load the shared libraries
       *
       *  @param  libraryNameList list of library names
       */
      StatusCode loadLibraries(const StringVector &libraryNameList);

      /** 
       *  @brief  Load the shared library
       *
       *  @param  libraryName the library name
       */
      StatusCode loadLibrary(const std::string &libraryName);

      /** 
       *  @brief  Get the plugin by name
       *
       *  @param  pluginName the plugin name to get
       */
      const Plugin *getPlugin(const std::string &pluginName) const;

      /** 
       *  @brief  Create a new instance of a specific plugin
       *
       *  @param  pluginName the plugin name
       */
      template <typename T>
      std::shared_ptr<T> create(const std::string &pluginName) const;

      /** 
       *  @brief  Whether the plug-in is registered within the plug-in manager
       *
       *  @param  pluginName the plugin name
       */
      bool isPluginRegistered(const std::string &pluginName) const;

      /** 
       *  @brief  Get the plug-in name list
       */
      StringVector pluginNames() const;

      /** 
       *  @brief  Dump registered plugin via logging
       */
      void dump() const;

    private:
      /** 
       *  @brief  Default constructor
       */
      PluginManager() = default;

      /** 
       *  @brief  Destructor
       */
      ~PluginManager();

      /** 
       *  @brief  Register the plug-in. Only available by plug-ins themselves
       *
       *  @param  the plugin instance to register
       */
      void registerPlugin(const Plugin *const plugin);

    private:
      /// The map of registered plugins
      PluginMap               m_pluginMap = {};
      ///< The list of loaded libraries
      std::vector<void*>      m_dlLibraries = {};
      ///
      static PluginManager*   m_instance;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief  Plugin class
     */
    class Plugin {
    public:
      /** 
       *  @brief  Constructor
       *
       *  @param  id the plugin identifier
       */
      template <typename T>
      inline Plugin(const std::string &id, const T& /* dummy */) :
        m_name(id),
        m_typeInfo(typeid(T)) {
        m_newFunction = [](){
          return std::make_shared<T>();
        };
        PluginManager::instance()->registerPlugin(this);
      }
      
      /**
       *  @brief  Get the type information on the internal plugin class
       */
      inline const std::type_index &type() const {
        return m_typeInfo;
      }

      /**
       *  @brief  Create a plugin instance
       */
      template <typename T>
      inline std::shared_ptr<T> create() const {
        return std::static_pointer_cast<T>(m_newFunction());
      }
      
      /**
       *  @brief  Get the plugin name
       */
      inline const std::string &name() const {
        return m_name;
      }
      
    private:
      /// The plugin name
      const std::string                            m_name;
      /// The plugin class type information
      const std::type_index                        m_typeInfo;
      /// A custom allocator function based on the underlying type
      std::function<std::shared_ptr<void>(void)>   m_newFunction = {nullptr};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline std::shared_ptr<T> PluginManager::create(const std::string &pluginName) const {
      auto plugin = getPlugin(pluginName);
      if (nullptr == plugin) {
        return std::shared_ptr<T>();
      }
      return plugin->create<T>();
    }

  }
  
}

#endif //  DQM4HEP_PLUGINMANAGER_H
