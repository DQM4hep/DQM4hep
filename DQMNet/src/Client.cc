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
#include "dqm4hep/Client.h"
#include "dqm4hep/RequestHandler.h"

namespace dqm4hep {

  namespace net {

    Client::Client() {
      DimClient::setNoDataCopy();
    }

    //-------------------------------------------------------------------------------------------------

    Client::~Client() {
      for (auto iter = m_serviceHandlerMap.begin(), endIter = m_serviceHandlerMap.end(); endIter != iter; ++iter)
        delete iter->second;

      m_serviceHandlerMap.clear();
    }

    //-------------------------------------------------------------------------------------------------

    void Client::queryServerInfo(const std::string &serverName, core::json &serverInfo) const {
      Buffer request;
      this->sendRequest("/" + serverName + "/info", request,
                        [&serverInfo](const Buffer &buffer) { serverInfo = core::json::parse(buffer.begin(), buffer.end()); });
    }

    //-------------------------------------------------------------------------------------------------

    bool Client::hasSubscribed(const std::string &name) const {
      return (m_serviceHandlerMap.end() != m_serviceHandlerMap.find(name));
    }

    //-------------------------------------------------------------------------------------------------

    unsigned int Client::numberOfSubscriptions(const std::string &serviceName) const {
      return m_serviceHandlerMap.count(serviceName);
    }

    //-------------------------------------------------------------------------------------------------

    void Client::notifyServerOnExit(const std::string &serverName) {
      DimClient::setExitHandler(serverName.c_str());
    }
  }
}
