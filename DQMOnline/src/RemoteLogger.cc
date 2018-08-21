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
#include "dqm4hep/RemoteLogger.h"
#include "dqm4hep/OnlineRoutes.h"

namespace dqm4hep {

  namespace online {

    RemoteLogger::RemoteLogger() {
      char hname[256];
      gethostname(hname, 256);
      m_hostname = hname;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::Logger::AppenderPtr RemoteLogger::make_shared() {
      return std::make_shared<RemoteLogger>();
    }
        
    //-------------------------------------------------------------------------------------------------
    
    void RemoteLogger::_sink_it(const spdlog::details::log_msg& msg) {
      m_message = {
        {"logger",   *msg.logger_name},
        {"level",   msg.level},
        {"message", msg.raw.str()},
        {"host",    m_hostname},
        {"pid",     dqm4hep::core::pid()}
      };
      m_client.sendCommand(OnlineRoutes::OnlineManager::collectLog(), m_message.dump());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void RemoteLogger::flush() {      
    }
    
  }

}

