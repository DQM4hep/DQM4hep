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
#include <dqm4hep/RunControlServer.h>
#include <dqm4hep/RunControlInterface.h>
#include <dqm4hep/OnlineRoutes.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/Logging.h>

using namespace dqm4hep::core;
using namespace dqm4hep::net;

namespace dqm4hep {

  namespace online {

    void RunControlServer::setName(const std::string &name) {
      m_runControl.setName(name);
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::setPassword(const std::string &pwd) {
      m_runControl.setPassword(pwd);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RunControlServer::setInterface(const std::string &name) {
      m_interfaceName = name;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RunControlServer::setUserParameters(const dqm4hep::core::StringMap &parameters) {
      m_userParameters = parameters;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    RunControl &RunControlServer::runControl() {
      return m_runControl;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::run() {
      m_stopFlag = false;
            
      // create user external interface (plugin)
      m_interface = PluginManager::instance()->create<RunControlInterface>(m_interfaceName);
      
      if( not m_interface ) {
        dqm_error( "Couldn't find run control interface '{0}' in plugin manager", m_interfaceName );
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
      }
  
      // create network interface
      m_server = std::make_shared<dqm4hep::net::Server>(m_runControl.name());

      m_pSorService = m_server->createService(OnlineRoutes::RunControl::sor(m_runControl.name()));
      m_runControl.onStartOfRun().connect(this, &RunControlServer::sor);

      m_pEorService = m_server->createService(OnlineRoutes::RunControl::eor(m_runControl.name()));
      m_runControl.onEndOfRun().connect(this, &RunControlServer::eor);

      m_server->createRequestHandler(OnlineRoutes::RunControl::status(m_runControl.name()), this, &RunControlServer::sendCurrentRun);
      
      m_interface->setServer(this);      
      m_interface->readSettings(m_userParameters);
      
      // start the server
      m_server->start();
      
      // run the user plugin
      if(m_interface->runBlocking())
      {
        // block here until stop called from outside
        m_interface->run();
      }
      else
      {
        // call run and sleep
        m_interface->run();
        
        while( ! m_stopFlag )
          dqm4hep::core::sleep(dqm4hep::core::TimeDuration(1));
      }
      
      m_server->stop();
      m_pSorService = nullptr;
      m_pEorService = nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::stop()
    {
      if(m_interface) 
        m_interface->stop();
        
      m_stopFlag = true;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::sor(dqm4hep::core::Run &r) {
      core::json jsonRun;
      r.toJson(jsonRun);
      m_pSorService->send(jsonRun.dump());
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::eor(const dqm4hep::core::Run &r) {
      core::json jsonRun;
      r.toJson(jsonRun);
      m_pEorService->send(jsonRun.dump());
    }

    //-------------------------------------------------------------------------------------------------

    void RunControlServer::sendCurrentRun(const Buffer &/*request*/, Buffer &response) {
      core::json jsonStatus, jsonRun;
      m_runControl.currentRun().toJson(jsonRun);
      jsonStatus = {
        {"running", m_runControl.isRunning()},
        {"run", jsonRun}
      };
      auto model = response.createModel<std::string>();
      std::string jsonDump(jsonStatus.dump());
      model->move(std::move(jsonDump));
      response.setModel(model);
    }

  }

}
