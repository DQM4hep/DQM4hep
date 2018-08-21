//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dqm4hep/ServiceHandler.h"
#include "dqm4hep/Service.h"

namespace dqm4hep {

  namespace net {

    ServiceHandler::~ServiceHandler() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &ServiceHandler::name() const {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    Client *ServiceHandler::client() const {
      return m_pClient;
    }

    //-------------------------------------------------------------------------------------------------

    ServiceHandler::UpdateSignal &ServiceHandler::onServiceUpdate() {
      return m_updateSignal;
    }

    //-------------------------------------------------------------------------------------------------

    void ServiceHandler::receiveServiceUpdated(const Buffer &data) {
      m_updateSignal.emit(data);
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    ServiceHandler::ServiceInfo::ServiceInfo(ServiceHandler *pHandler)
        : DimUpdatedInfo((char *)pHandler->name().c_str(), (void *)nullptr, 0), m_pHandler(pHandler) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    void ServiceHandler::ServiceInfo::infoHandler() {
      char *data = (char *)this->getData();
      int size = this->getSize();

      if (nullptr == data || size == 0)
        return;

      // std::string contents(data, size);
      Buffer buffer;
      buffer.adopt(data, size);
      m_pHandler->receiveServiceUpdated(buffer);
    }
  }
}
