//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dqm4hep/OnlineRoutes.h"

namespace dqm4hep {

  namespace online {

    const std::string OnlineRoutes::RunControl::status(const std::string &runControlName) {
      return "/dqm4hep/RunControl/" + runControlName + "/Status";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string OnlineRoutes::RunControl::sor(const std::string &runControlName) {
      return "/dqm4hep/RunControl/" + runControlName + "/SOR";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string OnlineRoutes::RunControl::eor(const std::string &runControlName) {
      return "/dqm4hep/RunControl/" + runControlName + "/EOR";
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::string OnlineRoutes::Application::serverName(const std::string &type, const std::string &name) {
      return "/dqm4hep/app/" + type + "/" + name;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string OnlineRoutes::Application::state(const std::string &type, const std::string &name) {
      return serverName(type, name) + "/state";
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    std::string OnlineRoutes::EventCollector::applicationType() {
      return "evtcol";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string OnlineRoutes::EventCollector::registerSource(const std::string &collector) {
      return OnlineRoutes::Application::serverName(applicationType(), collector) + "/register";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string OnlineRoutes::EventCollector::unregisterSource(const std::string &collector) {
      return OnlineRoutes::Application::serverName(applicationType(), collector) + "/unregister";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string OnlineRoutes::EventCollector::collectEvent(const std::string &collector) {
      return OnlineRoutes::Application::serverName(applicationType(), collector) + "/collect";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string OnlineRoutes::EventCollector::eventUpdate(const std::string &collector, const std::string &source) {
      return OnlineRoutes::Application::serverName(applicationType(), collector) + "/updates/" + source;
    }
    
    //-------------------------------------------------------------------------------------------------

    std::string OnlineRoutes::EventCollector::eventRequest(const std::string &collector) {
      return OnlineRoutes::Application::serverName(applicationType(), collector) + "/lastevt";
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    const std::string OnlineRoutes::OnlineManager::serverName() {
      return "/dqm4hep/onlineMgr";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string OnlineRoutes::OnlineManager::logs() {
      return "/dqm4hep/onlineMgr/logs";
    }
    
    //-------------------------------------------------------------------------------------------------

    const std::string OnlineRoutes::OnlineManager::appStats() {
      return "/dqm4hep/onlineMgr/appStats";
    }

    //-------------------------------------------------------------------------------------------------    

    const std::string OnlineRoutes::OnlineManager::collectLog() {
      return "/dqm4hep/onlineMgr/collectLog";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string OnlineRoutes::OnlineManager::collectAppStat() {
      return "/dqm4hep/onlineMgr/collectAppStat";
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    const std::string OnlineRoutes::ModuleApplication::applicationType() {
      return "module";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string OnlineRoutes::ModuleApplication::subscribe(const std::string &moduleName) {
      return OnlineRoutes::Application::serverName(applicationType(), moduleName) + "/subscribe";
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
  }

} 

