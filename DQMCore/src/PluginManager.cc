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
#include <dqm4hep/PluginManager.h>

// -- std headers
#include <cstdlib>
#include <dlfcn.h>

namespace dqm4hep {

  namespace core {

    PluginManager::~PluginManager() {
      // No memory clean here, as the plugin instances are static
      m_pluginMap.clear();
      
      for (auto iter : m_dlLibraries) {
        (void) dlclose(iter);
      }
      m_dlLibraries.clear();
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode PluginManager::loadLibraries() {
      // get the environment plug-in dll variable
      char *pPluginDllEnv = getenv("DQM4HEP_PLUGIN_DLL");

      if (pPluginDllEnv == nullptr) {
        dqm_debug("Environment variable DQM4HEP_PLUGIN_DLL not found ! \n"
                  "Set it before loading shared libraries. \n"
                  "Example : export DQM4HEP_PLUGIN_DLL=libPlugin1.so:libPlugin2.so");

        return STATUS_CODE_SUCCESS;
      }

      // split the string into a list
      std::string libraryConcatenateList(pPluginDllEnv);
      StringVector libraryList;

      dqm4hep::core::tokenize(libraryConcatenateList, libraryList, ":");

      // load the shared libraries
      return this->loadLibraries(libraryList);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode PluginManager::loadLibraries(const StringVector &libraryNameList) {
      for (auto iter = libraryNameList.begin(), endIter = libraryNameList.end(); endIter != iter; ++iter) {
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->loadLibrary(*iter));
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode PluginManager::loadLibrary(const std::string &libraryName) {
      size_t idx = libraryName.find_last_of("/");

      // the library basename, i.e. /path/to/libBlah.so --> libBlah.so
      std::string libBaseName(libraryName.substr(idx + 1));

      char *real_path = realpath(libraryName.c_str(), nullptr);

      if (nullptr != real_path) {
        dqm_info("<!-- Loading shared library : {0} ({1})-->", real_path, libBaseName);

        // use real_path
        free(real_path);
      } else {
        dqm_info("<!-- Loading shared library : {0} ({1})-->", libraryName, libBaseName);
      }

      void *pLibPointer = dlopen(libraryName.c_str(), RTLD_LAZY | RTLD_GLOBAL | RTLD_NODELETE);

      if (pLibPointer == nullptr) {
        dqm_error("<!-- ERROR loading shared library : {0}\n    ->    {1} -->", libraryName, dlerror());
        return STATUS_CODE_FAILURE;
      }
      
      m_dlLibraries.push_back(pLibPointer);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    const Plugin *PluginManager::getPlugin(const std::string &pluginName) const {
      if (!isPluginRegistered(pluginName))
        return nullptr;

      return m_pluginMap.find(pluginName)->second;
    }

    //-------------------------------------------------------------------------------------------------

    bool PluginManager::isPluginRegistered(const std::string &pluginName) const {
      return (m_pluginMap.find(pluginName) != m_pluginMap.end());
    }

    //-------------------------------------------------------------------------------------------------

    StringVector PluginManager::pluginNames() const {
      StringVector pluginNameList;

      for (auto iter = m_pluginMap.begin(), endIter = m_pluginMap.end(); endIter != iter; ++iter) {
        pluginNameList.push_back(iter->first);
      }

      return pluginNameList;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode PluginManager::registerPlugin(Plugin *pPlugin) {
      // null ptr case
      if (nullptr == pPlugin)
        return STATUS_CODE_INVALID_PTR;

      // check if the plug is already registered
      if (isPluginRegistered(pPlugin->pluginName())) {
        delete pPlugin;
        pPlugin = nullptr;
        return STATUS_CODE_ALREADY_PRESENT;
      }

      // try to register it
      if (!m_pluginMap.insert(PluginMap::value_type(pPlugin->pluginName(), pPlugin)).second) {
        delete pPlugin;
        pPlugin = nullptr;

        return STATUS_CODE_FAILURE;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    void PluginManager::dump() const {
      dqm_info("******************************************************");
      dqm_info("******** Plugin manager -- Registered plugins ********");
      for (auto plugin : m_pluginMap) {
        dqm_info("** Plugin [class={0}] : {1}", plugin.second->pluginName(), plugin.second->className());
      }
      dqm_info("******************************************************");
    }
  }
}
