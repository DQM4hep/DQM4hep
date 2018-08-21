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
#include "dqm4hep/ModuleApi.h"
#include "dqm4hep/ModuleApplication.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/Run.h"

namespace dqm4hep {

  namespace online {

    core::StatusCode ModuleApi::cd(const Module *const module) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      module->moduleApplication()->monitorElementManager()->cd();
      return core::STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    core::StatusCode ModuleApi::cd(const Module *const module, const std::string &dirName) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->cd(dirName);
    }

    //-------------------------------------------------------------------------------------------------

    core::StatusCode ModuleApi::mkdir(const Module *const module, const std::string &dirName) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->mkdir(dirName);
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &ModuleApi::pwd(const Module *const module) {
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->pwd();
    }

    //-------------------------------------------------------------------------------------------------

    core::StatusCode ModuleApi::goUp(const Module *const module) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->goUp();
    }

    //-------------------------------------------------------------------------------------------------

    core::StatusCode ModuleApi::rmdir(const Module *const module, const std::string &dirName) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->rmdir(dirName);
    }

    //-------------------------------------------------------------------------------------------------

    bool ModuleApi::dirExists(const Module *const module, const std::string &dirName) {
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->dirExists(dirName);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::StatusCode ModuleApi::dump(const Module *const module) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      module->moduleApplication()->monitorElementManager()->dumpStorage();
      return core::STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    core::StatusCode ModuleApi::getMonitorElements(const Module *const module, OnlineElementPtrList &monitorElementList) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      module->moduleApplication()->monitorElementManager()->getMonitorElements(monitorElementList);
      return core::STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    core::StatusCode ModuleApi::getMonitorElement(const Module *const module, const std::string &monitorElementName, OnlineElementPtr &monitorElement) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->getMonitorElement(monitorElementName, monitorElement);
    }

    //-------------------------------------------------------------------------------------------------

    core::StatusCode ModuleApi::getMonitorElement(const Module *const module, const std::string &dirName, const std::string &monitorElementName, OnlineElementPtr &monitorElement) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->getMonitorElement(dirName, monitorElementName, monitorElement);
    }

    //-------------------------------------------------------------------------------------------------

    OnlineElementPtr ModuleApi::getMonitorElement(const Module *const module, const std::string &monitorElementName) {
      OnlineElementPtr monitorElement;
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, module->moduleApplication()->monitorElementManager()->getMonitorElement(monitorElementName, monitorElement));
      return monitorElement;
    }

    //-------------------------------------------------------------------------------------------------

    OnlineElementPtr ModuleApi::getMonitorElement(const Module *const module, const std::string &dirName, const std::string &monitorElementName) {
      OnlineElementPtr monitorElement;
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, module->moduleApplication()->monitorElementManager()->getMonitorElement(dirName, monitorElementName, monitorElement));
      return monitorElement;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    core::StatusCode ModuleApi::resetMonitorElements(const Module *const module) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      module->moduleApplication()->monitorElementManager()->resetMonitorElements();
      return core::STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    core::StatusCode ModuleApi::addQualityTest(const Module *const module, OnlineElementPtr monitorElement, const std::string &qualityTestName) {
      RETURN_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::checkModule(module));
      return module->moduleApplication()->monitorElementManager()->addQualityTest(monitorElement->path(), monitorElement->name(), qualityTestName);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::StatusCode ModuleApi::checkModule(const Module *const module) {
      if(nullptr == module) {
        dqm_error( "ModuleApi::checkModule: module is nullptr!" );
        return core::STATUS_CODE_INVALID_PTR;
      }
      if(nullptr == module->moduleApplication()) {
        dqm_error( "ModuleApi::checkModule: module application is nullptr!" );
        return core::STATUS_CODE_NOT_INITIALIZED;
      }
      return core::STATUS_CODE_SUCCESS;
    }

  }

}

