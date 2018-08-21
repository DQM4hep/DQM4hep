//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_REMOTE_LOGGER_H
#define DQM4HEP_REMOTE_LOGGER_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/Logger.h"
#include "dqm4hep/json.h"
#include "dqm4hep/Client.h"
#include "spdlog/sinks/base_sink.h"

namespace dqm4hep {

  namespace online {
    
    /**
     *  @brief  RemoteLogger class
     */
    class RemoteLogger : public spdlog::sinks::base_sink<std::mutex> {
    public:
      /**
       *  @brief  Constructor
       */
      RemoteLogger();
      
      /**
       *  @brief  Create a shared pointer of RemoteLogger
       */
      static core::Logger::AppenderPtr make_shared();
      
      /**
       *  @brief  Log a message
       *  
       *  @param  msg the spdlog log message 
       */
      void _sink_it(const spdlog::details::log_msg& msg) override;
      
      /**
       *  @brief  Flush the message
       */
      void flush() override;
      
    private:
      core::json     m_message = {};       ///< The log message to send as json
      std::string    m_hostname = {""};    ///< The current host name
      net::Client    m_client = {};        ///< The net client interface
    };

  }

} 

#endif  //  DQM4HEP_REMOTE_LOGGER_H
