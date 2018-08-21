//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef SERVICE_H
#define SERVICE_H

// -- std headers
#include <string>
#include <typeinfo>

// -- dim headers
#include "dis.hxx"

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/NetBuffer.h"

namespace dqm4hep {

  namespace net {

    class Server;

    class Service {
      friend class Server;

    public:
      /**
       * Get the service name
       */
      const std::string &name() const;

      /**
       * Get the server in which the service is declared
       */
      Server *server() const;

      /**
       * Send a simple value
       */
      template <typename T>
      void send(const T &value);

      /**
       * Send an array of simple values
       */
      template <typename T>
      void sendArray(const T *value, size_t nElements);

      /**
       * Send a raw buffer. The buffer is owned by the caller
       */
      void sendBuffer(const void *ptr, size_t size);

      /**
       * Send a simple value to a specific client
       */
      template <typename T>
      void send(const T &value, int clientId);

      /**
       * Send an array of simple values to a specific client
       */
      template <typename T>
      void sendArray(const T *value, size_t nElements, int clientId);

      /**
       * Send a raw buffer to a specific client. The buffer is owned by the caller
       */
      void sendBuffer(const void *ptr, size_t size, int clientId);

      /**
       * Send a simple value to a specific list of clients
       */
      template <typename T>
      void send(const T &value, const std::vector<int> &clientIds);

      /**
       * Send an array of simple values to a specific list of clients
       */
      template <typename T>
      void sendArray(const T *value, size_t nElements, const std::vector<int> &clientIds);

      /**
       * Send a raw buffer to a specific list of clients. The buffer is owned by the caller
       */
      void sendBuffer(const void *ptr, size_t size, const std::vector<int> &clientIds);

    private:
      /**
       * Constructor with service name
       *
       * @param pServer the server that owns the service instance
       * @param name the service name
       */
      Service(Server *pServer, const std::string &name);
      Service(const Service&) = delete;
      Service& operator=(const Service&) = delete;

      /**
       * Destructor
       */
      virtual ~Service();

      /**
       * Create the actual service connection
       */
      void connectService();

      /**
       * Remove the actual service connection
       */
      void disconnectService();

      /**
       * Whether the service is connected
       */
      bool isServiceConnected() const;

      /**
       *
       */
      void sendData(const Buffer &buffer, const std::vector<int> &clientIds);

    private:
      DimService         *m_pService = {nullptr};      ///< The service implementation
      std::string         m_name = {""};               ///< The service name
      Server             *m_pServer = {nullptr};       ///< The server in which the service is declared
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Service::send(const T &value) {
      Buffer buffer;
      auto model = buffer.createModel<T>();
      model->copy(value);
      buffer.setModel(model);
      this->sendData(buffer, std::vector<int>());
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Service::sendArray(const T *value, size_t nElements) {
      Buffer buffer(value, nElements);
      this->sendData(buffer, std::vector<int>());
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Service::send(const T &value, int clientId) {
      Buffer buffer(value);
      this->sendData(buffer, std::vector<int>(1, clientId));
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Service::sendArray(const T *value, size_t nElements, int clientId) {
      Buffer buffer(value, nElements);
      this->sendData(buffer, std::vector<int>(1, clientId));
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Service::send(const T &value, const std::vector<int> &clientIds) {
      Buffer buffer(value);
      this->sendData(buffer, clientIds);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Service::sendArray(const T *value, size_t nElements, const std::vector<int> &clientIds) {
      Buffer buffer(value, nElements);
      this->sendData(buffer, clientIds);
    }
  }
}

#endif //  SERVICE_H
