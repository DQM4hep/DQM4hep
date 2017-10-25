/// \file Server.h
/*
 *
 * Server.h header template automatically generated by a class generator
 * Creation date : sam. d�c. 3 2016
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


#ifndef SERVER_H
#define SERVER_H

// -- dqm4hep headers
#include "dqm4hep/Service.h"
#include "dqm4hep/RequestHandler.h"

// -- dim headers
#include "dis.hxx"

namespace dqm4hep {

  namespace net {

    /**
     * Server class
     *
     * Main interface for service and request handler registration.
     * User can create services using the createService() method.
     * Request from client side can be handled by registering a
     * request handler by providing a class method to handle the
     * request and fill a reponse.
     *
     * All created services and request handlers are started after
     * server startup.
     */
    class Server
    {
    public:
      /**
       * Constructor
       *
       * @param name the server name
       */
      Server(const std::string &name);

      /**
       * Destructor
       */
      ~Server();

      /**
       * Get the server name
       */
      const std::string &name() const;

      /**
       * Start serving services and handling requests
       */
      void start();

      /**
       * Stop serving services and handling requests
       */
      void stop();

      /**
       * Whether the server is running
       */
      bool isRunning() const;

      /**
       * Close all service and request handlers.
       * Called from destructor
       */
      void clear();

      /**
       * Create a new service.
       * @param name the service name
       */
      Service *createService(const std::string &name);

      /**
       * Create a new request handler
       *
       * @param name the request handler name
       * @param pController the class instance that will handle the request
       * @param function the class method that will treat the request and provide a response
       */
      template <typename Controller >
      void createRequestHandler(const std::string &name,
          Controller *pController, void (Controller::*function)(const Buffer &request, Buffer &response));

      /**
       * Create a new command handler
       *
       * @param name the command handler name
       * @param pController the class instance that will handle the command
       * @param function the class method that will treat the command
       */
      template <typename Controller>
      void createCommandHandler(const std::string &name,
          Controller *pController, void (Controller::*function)(const Buffer &command));

      /**
       * Whether the target service is registered in this server
       *
       * @param name the service name
       */
      bool isServiceRegistered(const std::string &name) const;

      /**
       * Whether the request handler is registered in this server
       *
       * @param type the request handler type
       * @param name the request handler name
       */
      bool isRequestHandlerRegistered(const std::string &name) const;

      /**
       * Whether the command handler is registered in this server
       *
       * @param type the command handler type
       * @param name the command handler name
       */
      bool isCommandHandlerRegistered(const std::string &name) const;

      /**
       * Start a target service
       *
       * @param name the service name
       */
      void startService(const std::string &name);

      /**
       * Stop a target service
       *
       * @param name the service name
       */
      void stopService(const std::string &name);

      /**
       * Start a target request handler
       *
       * @param type the request handler type
       * @param name the request handler name
       */
      void startRequestHandler(const std::string &name);

      /**
       * Stop a target request handler
       *
       * @param type the request handler type
       * @param name the request handler name
       */
      void stopRequestHandler(const std::string &name);

      /**
       * Start a target request handler
       *
       * @param type the command type
       * @param name the command name
       */
      void startCommandHandler(const std::string &name);

      /**
       * Stop a target command handler
       *
       * @param type the command handler type
       * @param name the command handler name
       */
      void stopCommandHandler(const std::string &name);

      template <typename Controller>
      void stopCommandHandler(const std::string &name, Controller *pController);

      template <typename Controller>
      void stopCommandHandler(const std::string &name, Controller *pController, void (Controller::*function)(const Buffer &command));

      /**
       * Get a created service in this server
       *
       * @param name the service name
       */
      Service *service(const std::string &name) const;

      /**
       * Get the dim dns node.
       *  First look at DimServer::getDnsNode() then
       *  environment variable "DIM_DNS_NODE"
       */
      static std::string dnsNode();

      /**
       * Get the dim dns port
       */
      static int dnsPort();

      /**
       * Get the list of running servers
       */
      static std::vector<std::string> runningServers();

      /**
       * Whether the target server is already running on the network
       *
       * @param serverName the 'short' server name
       */
      static bool isServerRunning(const std::string &serverName);

      /**
       * Whether the service is already running on the network
       *
       * @param name the service name
       */
      static bool serviceAlreadyRunning(const std::string &name);

      /**
       * Whether the request handler is already running on the network
       *
       * @param type the request handler type
       * @param name the request handler name
       */
      static bool requestHandlerAlreadyRunning(const std::string &name);

      /**
       * Whether the command handler is already running on the network
       *
       * @param type the command handler type
       * @param name the command handler name
       */
      static bool commandHandlerAlreadyRunning(const std::string &name);

    private:
      /**
       * Callback function to treat the server info request
       *
       * @param request the request
       * @param response the response to receive
       */
      void handleServerInfoRequest(const Buffer &, Buffer &response);

      RequestHandler *requestHandler(const std::string &name) const;

      CommandHandler *commandHandler(const std::string &name) const;

    private:
      typedef std::map<std::string, Service *>         ServiceMap;
      typedef std::map<std::string, RequestHandler *>  RequestHandlerMap;
      typedef std::map<std::string, CommandHandler *>  CommandHandlerMap;

      std::string                                    m_name;                 ///< The short server name
      bool                                           m_started;              ///< Whether the server has been started
      ServiceMap                                     m_serviceMap;           ///< The map of registered services
      RequestHandlerMap                              m_requestHandlerMap;    ///< The map of registered request handlers
      CommandHandlerMap                              m_commandHandlerMap;    ///< The map of registered command handlers
      RequestHandler                                 m_serverInfoHandler;    ///< The built-in request handler for server info
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Server::createRequestHandler(const std::string &name,
        Controller *pController, void (Controller::*function)(const Buffer &request, Buffer &response))
    {
      auto findIter = m_requestHandlerMap.find(name);

      if(findIter != m_requestHandlerMap.end())
        throw std::runtime_error("Server::createRequestHandler(): request handler '" + name + "' already exists in this client");

      if(Server::requestHandlerAlreadyRunning(name))
        throw std::runtime_error("Server::createRequestHandler(): request handler '" + name + "' already running on network");

      // first insert nullptr, then create request handler
      std::pair<RequestHandlerMap::iterator, bool> inserted = m_requestHandlerMap.insert(RequestHandlerMap::value_type(name, nullptr));

      if(inserted.second)
      {
        RequestHandler *pRequestHandler = new RequestHandler(this, name, pController, function);
        inserted.first->second = pRequestHandler;

        if(this->isRunning())
          pRequestHandler->startHandlingRequest();
      }
      else
        throw;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Server::createCommandHandler(const std::string &name,
        Controller *pController, void (Controller::*function)(const Buffer &command))
    {
      auto findIter = m_commandHandlerMap.find(name);

      if(findIter != m_commandHandlerMap.end())
      {
        findIter->second->onCommand().connect(pController, function);

        if(this->isRunning())
          findIter->second->startHandlingCommands();

        return;
      }

      if(Server::commandHandlerAlreadyRunning(name))
        throw std::runtime_error("Server::createCommandHandler(): command handler '" + name + "' already running on network");

      // first insert nullptr, then create command handler
      std::pair<CommandHandlerMap::iterator, bool> inserted = m_commandHandlerMap.insert(CommandHandlerMap::value_type(name, nullptr));

      if(inserted.second)
      {
        CommandHandler *pCommandHandler = new CommandHandler(this, name, pController, function);
        inserted.first->second = pCommandHandler;

        if(this->isRunning())
          pCommandHandler->startHandlingCommands();
      }
      else
        throw;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Server::stopCommandHandler(const std::string &name, Controller *pController)
    {
      auto findIter = m_commandHandlerMap.find(name);

      if(findIter != m_commandHandlerMap.end())
      {
        findIter->second->onCommand().disconnect(pController);

        if(!findIter->second->onCommand().hasConnection())
          m_commandHandlerMap.erase(findIter);
      }
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Server::stopCommandHandler(const std::string &name, Controller *pController, void (Controller::*function)(const Buffer &command))
    {
      auto findIter = m_commandHandlerMap.find(name);

      if(findIter != m_commandHandlerMap.end())
      {
        findIter->second->onCommand().disconnect(pController, function);

        if(!findIter->second->onCommand().hasConnection())
          m_commandHandlerMap.erase(findIter);
      }
    }

  }

}

#endif  //  SERVER_H
