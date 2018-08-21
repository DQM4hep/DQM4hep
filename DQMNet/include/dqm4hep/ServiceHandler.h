//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef SERVICEHANDLER_H
#define SERVICEHANDLER_H

// -- dim headers
#include "dic.hxx"

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/NetBuffer.h"
#include "dqm4hep/Signal.h"
#include "dqm4hep/json.h"

namespace dqm4hep {

  namespace net {

    class Client;

    /**
     *
     */
    class ServiceHandler {
      friend class Client;

    public:
      typedef core::Signal<const Buffer &> UpdateSignal;

      /**
       * Get the service name
       */
      const std::string &name() const;

      /**
       * Get the client interface
       */
      Client *client() const;

      /**
       * [onServiceUpdate description]
       * @return [description]
       */
      UpdateSignal &onServiceUpdate();

    private:
      /**
       * Constructor
       *
       * @param pClient the client that owns the service handler
       * @param name the service name
       */
      template <typename Controller>
      ServiceHandler(Client *pClient, const std::string &name, Controller *pController,
                     void (Controller::*function)(const Buffer &));

      ServiceHandler() = delete;
      ServiceHandler(const ServiceHandler&) = delete;
      ServiceHandler& operator=(const ServiceHandler&) = delete;

      /**
       * Destructor
       */
      ~ServiceHandler();

    private:
      /** ServiceInfo class.
       *
       *  The concrete dim updated info implementation
       */
      class ServiceInfo : public DimUpdatedInfo {
      public:
        /** Contructor
         */
        ServiceInfo(ServiceHandler *pHandler);
        ServiceInfo() = delete;
        ServiceInfo(const ServiceInfo&) = delete;
        ServiceInfo& operator=(const ServiceInfo&) = delete;

        /** The dim rpc handler
         */
        void infoHandler() override;

      private:
        ServiceHandler *m_pHandler = {nullptr};
      };

      /**
       * [receiveServiceUpdated description]
       * @param contents [description]
       */
      void receiveServiceUpdated(const Buffer &);

    private:
      std::string          m_name = {""};           ///< The request handler name
      Client              *m_pClient = {nullptr};   ///< The client manager
      ServiceInfo          m_serviceInfo;
      UpdateSignal         m_updateSignal = {};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline ServiceHandler::ServiceHandler(Client *pClient, const std::string &sname, Controller *pController,
                                          void (Controller::*function)(const Buffer &))
        : m_name(sname), m_pClient(pClient), m_serviceInfo(this) {
      m_updateSignal.connect(pController, function);
    }
  }
}

#endif //  SERVICEHANDLER_H
