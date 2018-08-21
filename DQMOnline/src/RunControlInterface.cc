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
#include <dqm4hep/RunControlInterface.h>
#include <dqm4hep/RunControlServer.h>
#include <dqm4hep/Run.h>
#include <dqm4hep/Logging.h>

using namespace dqm4hep::core;

namespace dqm4hep {

  namespace online {

    void RunControlInterface::startNewRun(const dqm4hep::core::Run &r, const std::string &password) {
      if(!m_pServer) {
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
      }
      m_pServer->runControl().startNewRun(r, password);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RunControlInterface::endCurrentRun(const dqm4hep::core::StringMap &parameters, const std::string &password) {
      if(!m_pServer) {
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
      }
      m_pServer->runControl().endCurrentRun(parameters, password);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    StartOfRunSignal &RunControlInterface::onStartOfRun() {
      if(!m_pServer) {
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
      }
      return m_pServer->runControl().onStartOfRun();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    EndOfRunSignal &RunControlInterface::onEndOfRun() {
      if(!m_pServer) {
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
      }
      return m_pServer->runControl().onEndOfRun();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const RunControl& RunControlInterface::runControl() const {
      if(!m_pServer) {
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
      }
      return m_pServer->runControl();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RunControlInterface::setServer(RunControlServer *srv) {
      m_pServer = srv;
    }

  }

}
