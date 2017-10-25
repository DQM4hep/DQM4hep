/// \file Client.h
/*
 *
 * Client.h header template automatically generated by a class generator
 * Creation date : dim. d�c. 4 2016
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef CLIENT_H
#define CLIENT_H

// -- dqm4hep headers
#include "dqm4hep/DQMNet.h"
#include "dqm4hep/RequestHandler.h"
#include "dqm4hep/ServiceHandler.h"
#include "dqm4hep/Service.h"

// -- json headers
#include "json/json.h"

// -- dim headers
#include "dic.hxx"

// -- std headers
#include <algorithm>

namespace dqm4hep {

  namespace net {

    /**
     * Client class
     *
     * Main interface to server. User can send request to
     * a server with or without expecting a response using
     * the sendRequest() function. User can also subscribe to
     * a particular service run on a server by using the
     * subscribe() method and by providing a callback function.
     */
    class Client
    {
    public:
      /**
       * Constructor
       */
      Client();

      /**
       * Destructor
       */
      ~Client();

      /**
       * Query server information
       *
       * @param serverName the 'short' server name
       * @param serverInfo the json value describing the server information
       */
      void queryServerInfo(const std::string &serverName, Json::Value &serverInfo) const;

      /**
       * Send a command. Do not wait for any acknowledgment
       *
       * @param name the request name
       * @param command the request content
       */
      template <typename Request>
      void sendRequest(const std::string &name, const Request &request) const;

      /**
       * Send a request. Wait for the server response (blocking)

       * @param name the request name
       * @param request the request to send
       * @param response the response to receive from the server
       */
      template <typename Request>
      void sendRequest(const std::string &name, const Request &request, Buffer &response) const;

      /**
       * Send a command.
       *
       * @param name the command name
       * @param command the command to send
       * @param blocking whether to wait for command reception on server side
       */
      template <typename Command>
      void sendCommand(const std::string &name, const Command &command, bool blocking = false) const;

      /**
       * Subscribe to target service
       *
       * @param serviceName the service name
       * @param pController the class instance that will receive the service updates
       * @param function the class method that will receive the service update
       *
       * @code{.cpp}
       * // the class callback method
       * void MyClass::myfunction(const int &integer) { ... }
       * Client *pClient = new Client();
       * pClient->subscribe<int>("service-type", "service-name", pMyClassInstance, &MyClass::myfunction)
       * @endcode
       */
      template <typename Controller>
      void subscribe(const std::string &serviceName, Controller *pController, void (Controller::*function)(const Buffer &));

      /**
       *
       */
      template <typename Controller>
      void unsubscribe(const std::string &serviceName, Controller *pController, void (Controller::*function)(const Buffer &));

      /**
       * Whether this client already registered a service subscription
       *
       * @param serviceName the service name
       */
      bool hasSubscribed(const std::string &serviceName) const;

      /**
       * [numberOfSubscriptions description]
       * @param  serviceName [description]
       * @return             [description]
       */
      unsigned int numberOfSubscriptions(const std::string &serviceName) const;

    private:
      typedef std::map<std::string, ServiceHandler *>      ServiceHandlerMap;
      typedef std::vector<ServiceHandler *>                     ServiceHandlerList;
      ServiceHandlerMap            m_serviceHandlerMap;        ///< The service map
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename Request>
    inline void Client::sendRequest(const std::string &name, const Request &request) const
    {
      DimRpcInfo rpcInfo(const_cast<char*>(name.c_str()), (void*)nullptr, 0);
      Buffer contents(request);
      rpcInfo.setData((void*)contents.begin(), contents.size());
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void Client::sendRequest(const std::string &name, const Buffer &request) const
    {
      DimRpcInfo rpcInfo(const_cast<char*>(name.c_str()), (void*)nullptr, 0);
      Buffer contents(request.begin(), request.size());
      rpcInfo.setData((void*)contents.begin(), contents.size());
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Request>
    inline void Client::sendRequest(const std::string &name, const Request &request, Buffer &response) const
    {
      DimRpcInfo rpcInfo(const_cast<char*>(name.c_str()), (void*)nullptr, 0);
      Buffer contents(request);

      // send request
      rpcInfo.setData((void*)contents.begin(), contents.size());

      // wait for answer from server
      char *data = (char*)rpcInfo.getData();
      int size = rpcInfo.getSize();

      if(nullptr != data && 0 != size)
        response.adopt(data, size);
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void Client::sendRequest(const std::string &name, const Buffer &request, Buffer &response) const
    {
      DimRpcInfo rpcInfo(const_cast<char*>(name.c_str()), (void*)nullptr, 0);
      Buffer contents(request.begin(), request.size());

      // send request
      rpcInfo.setData((void*)contents.begin(), contents.size());

      // wait for answer from server
      char *data = (char*)rpcInfo.getData();
      int size = rpcInfo.getSize();

      if(nullptr != data && 0 != size)
        response.adopt(data, size);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Command>
    inline void Client::sendCommand(const std::string &name, const Command &command, bool blocking) const
    {
      Buffer buffer(command);

      if(blocking)
      {
        DimClient::sendCommand(const_cast<char*>(name.c_str()), (void*)buffer.begin(), buffer.size());
      }
      else
      {
        DimClient::sendCommandNB(const_cast<char*>(name.c_str()), (void*)buffer.begin(), buffer.size());
      }
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void Client::sendCommand(const std::string &name, const Buffer &buffer, bool blocking) const
    {
      if(blocking)
      {
        DimClient::sendCommand(const_cast<char*>(name.c_str()), (void*)buffer.begin(), buffer.size());
      }
      else
      {
        DimClient::sendCommandNB(const_cast<char*>(name.c_str()), (void*)buffer.begin(), buffer.size());
      }
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Client::subscribe(const std::string &name, Controller *pController, void (Controller::*function)(const Buffer &))
    {
      auto findIter = m_serviceHandlerMap.find(name);

      if(findIter != m_serviceHandlerMap.end())
      {
        findIter->second->onServiceUpdate().connect(pController, function);
        return;
      }

      m_serviceHandlerMap.insert(
        ServiceHandlerMap::value_type(name, new ServiceHandler(this, name, pController, function))
      );
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Client::unsubscribe(const std::string &serviceName, Controller *pController, void (Controller::*function)(const Buffer &))
    {
      for(auto iter = m_serviceHandlerMap.begin(), endIter = m_serviceHandlerMap.end() ; endIter != iter ; ++iter)
      {
        if(serviceName != iter->first)
          continue;

        if(iter->second->onServiceUpdate().disconnect(pController, function))
          break;
      }
    }

  }

}

#endif  //  CLIENT_H
