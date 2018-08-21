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
#include "dqm4hep/Service.h"

namespace dqm4hep {

  namespace net {

    Service::Service(Server *pServer, const std::string &sname) : 
      m_name(sname), 
      m_pServer(pServer) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    Service::~Service() {
      this->disconnectService();
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Service::name() const {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    Server *Service::server() const {
      return m_pServer;
    }

    //-------------------------------------------------------------------------------------------------

    void Service::connectService() {
      if (!this->isServiceConnected()) {
        m_pService = new DimService((char *)m_name.c_str(), "C", (char *)NullBuffer::buffer, NullBuffer::size);
      }
    }

    //-------------------------------------------------------------------------------------------------

    void Service::disconnectService() {
      if (this->isServiceConnected()) {
        delete m_pService;
        m_pService = nullptr;
      }
    }

    //-------------------------------------------------------------------------------------------------

    bool Service::isServiceConnected() const {
      return (m_pService != nullptr);
    }

    //-------------------------------------------------------------------------------------------------

    void Service::sendBuffer(const void *ptr, size_t size) {
      Buffer buffer;
      buffer.adopt((const char *)ptr, size);
      this->sendData(buffer, std::vector<int>());
    }

    //-------------------------------------------------------------------------------------------------

    void Service::sendBuffer(const void *ptr, size_t size, int clientId) {
      Buffer buffer;
      buffer.adopt((const char *)ptr, size);
      this->sendData(buffer, std::vector<int>(1, clientId));
    }

    //-------------------------------------------------------------------------------------------------

    void Service::sendBuffer(const void *ptr, size_t size, const std::vector<int> &clientIds) {
      Buffer buffer;
      buffer.adopt((const char *)ptr, size);
      this->sendData(buffer, clientIds);
    }

    //-------------------------------------------------------------------------------------------------

    void Service::sendData(const Buffer &buffer, const std::vector<int> &clientIds) {
      if (!this->isServiceConnected())
        throw; // TODO implement exceptions

      if (clientIds.empty()) {
        m_pService->updateService((void *)buffer.begin(), buffer.size());
        m_pService->itsData = (void *)NullBuffer::buffer;
        m_pService->itsSize = NullBuffer::size;
      } else {
        std::vector<int> clientIdList(clientIds);

        if (clientIdList.back() != 0)
          clientIdList.push_back(0);

        int *clientIdsArray = &clientIdList[0];
        m_pService->selectiveUpdateService((void *)buffer.begin(), buffer.size(), clientIdsArray);
        m_pService->itsData = (void *)NullBuffer::buffer;
        m_pService->itsSize = NullBuffer::size;
      }
    }
  }
}
