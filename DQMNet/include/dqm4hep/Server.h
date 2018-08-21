//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef SERVER_H
#define SERVER_H

// -- dqm4hep headers
#include <dqm4hep/NetBuffer.h>
#include <dqm4hep/RequestHandler.h>
#include <dqm4hep/Service.h>
#include <dqm4hep/Signal.h>

// -- dim headers
#include <dis.hxx>

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
    class Server : public DimServer {
    public:
      /**
       *  @brief  Constructor
       *
       *  @param  name the server name
       */
      Server(const std::string &name);
      Server() = delete;
      Server(const Server&) = delete;
      Server& operator=(const Server&) = delete;

      /**
       *  @brief  Destructor
       */
      ~Server() override;

      /**
       *  @brief  Get the server name
       */
      const std::string &name() const;

      /**
       * @brief  Start serving services and handling requests
       */
      void start();

      /**
       *  @brief  Stop serving services and handling requests
       */
      void stop();

      /**
       *  @brief  Whether the server is running
       */
      bool isRunning() const;

      /**
       *  @brief  Close all service and request handlers.
       *          Called from destructor
       */
      void clear();

      /**
       *  @brief  Create a new service.
       *
       *  @param  name the service name
       */
      Service *createService(const std::string &name);

      /**
       *  @brief  Create a new request handler
       *
       *  @param  name the request handler name
       *  @param  pController the class instance that will handle the request
       *  @param  function the class method that will treat the request and provide a response
       */
      template <typename Controller>
      void createRequestHandler(const std::string &name, Controller *pController,
                                void (Controller::*function)(const Buffer &request, Buffer &response));

      /**
       *  @brief  Create a new command handler
       *
       *  @param  name the command handler name
       *  @param  pController the class instance that will handle the command
       *  @param  function the class method that will treat the command
       */
      template <typename Controller>
      void createCommandHandler(const std::string &name, Controller *pController,
                                void (Controller::*function)(const Buffer &command));

      /**
       *  @brief  Whether the target service is registered in this server
       *
       *  @param  name the service name
       */
      bool isServiceRegistered(const std::string &name) const;

      /**
       *  @brief  Whether the request handler is registered in this server
       *
       *  @param  name the request handler name
       */
      bool isRequestHandlerRegistered(const std::string &name) const;

      /**
       *  @brief  Whether the command handler is registered in this server
       *
       *  @param  name the command handler name
       */
      bool isCommandHandlerRegistered(const std::string &name) const;

      /**
       *  @brief  Start a target service
       *
       *  @param  name the service name
       */
      void startService(const std::string &name);

      /**
       *  @brief  Stop a target service
       *
       *  @param  name the service name
       */
      void stopService(const std::string &name);

      /**
       *  @brief  Start a target request handler
       *
       *  @param  name the request handler name
       */
      void startRequestHandler(const std::string &name);

      /**
       * @brief  Stop a target request handler
       *
       *  @param  name the request handler name
       */
      void stopRequestHandler(const std::string &name);

      /**
       *  @brief  Start a target request handler
       *
       *  @param name the command name
       */
      void startCommandHandler(const std::string &name);

      /**
       *  @brief  Stop a target command handler
       *
       *  @param  name the command handler name
       */
      void stopCommandHandler(const std::string &name);

      /**
       *  @brief  Stop a specific command handler
       *
       *  @param  name the command handler name
       *  @param  pController the command handler controller
       */
      template <typename Controller>
      void stopCommandHandler(const std::string &name, Controller *pController);

      /**
       *  @brief  Stop a specific command handler
       *
       *  @param  name the command handler name
       *  @param  pController the command handler controller
       *  @param  function the command handler function
       */
      template <typename Controller>
      void stopCommandHandler(const std::string &name, Controller *pController,
                              void (Controller::*function)(const Buffer &command));

      /**
       *  @brief  Get a created service in this server
       *
       *  @param  name the service name
       */
      Service *service(const std::string &name) const;

      /**
       *  @brief  Get the signal processed on client exit
       */
      core::Signal<int> &onClientExit();

      /**
       *  @brief  Get the client id. To be used inside callbacks
       */
      int clientId() const;

      /**
      *  @brief  Get the dim dns node.
       *         First look at DimServer::getDnsNode() then
       *         environment variable "DIM_DNS_NODE"
       */
      static std::string dnsNode();

      /**
       *  @brief  Get the dim dns port
       */
      static int dnsPort();

      /**
       *  @brief  Get the list of running servers
       */
      static std::vector<std::string> runningServers();

      /**
       *  @brief  Whether the target server is already running on the network
       *
       *  @param  serverName the 'short' server name
       */
      static bool isServerRunning(const std::string &serverName);

      /**
       *  @brief  Whether the service is already running on the network
       *
       *  @param  name the service name
       */
      static bool serviceAlreadyRunning(const std::string &name);

      /**
       *  @brief  Whether the request handler is already running on the network
       *
       *  @param  name the request handler name
       */
      static bool requestHandlerAlreadyRunning(const std::string &name);

      /**
       *  @brief  Whether the command handler is already running on the network
       *
       *  @param  name the command handler name
       */
      static bool commandHandlerAlreadyRunning(const std::string &name);

    private:
      void handleServerInfoRequest(const Buffer &, Buffer &response);
      RequestHandler *requestHandler(const std::string &name) const;
      CommandHandler *commandHandler(const std::string &name) const;
      void clientExitHandler() override;
      void commandHandler() override {};

    private:
      typedef std::map<std::string, Service *> ServiceMap;
      typedef std::map<std::string, RequestHandler *> RequestHandlerMap;
      typedef std::map<std::string, CommandHandler *> CommandHandlerMap;

      std::string                   m_name = {""};             ///< The short server name
      bool                          m_started = {false};       ///< Whether the server has been started
      ServiceMap                    m_serviceMap = {};         ///< The map of registered services
      RequestHandlerMap             m_requestHandlerMap = {};  ///< The map of registered request handlers
      CommandHandlerMap             m_commandHandlerMap = {};  ///< The map of registered command handlers
      RequestHandler               *m_serverInfoHandler = {nullptr};  ///< The built-in request handler for server info
      core::Signal<int>             m_clientExitSignal = {};   ///< The signal emitted whenever a client exits
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Server::createRequestHandler(const std::string &rname, Controller *pController,
                                             void (Controller::*function)(const Buffer &request, Buffer &response)) {
      auto findIter = m_requestHandlerMap.find(rname);

      if (findIter != m_requestHandlerMap.end())
        throw std::runtime_error("Server::createRequestHandler(): request handler '" + rname +
                                 "' already exists in this client");

      if (Server::requestHandlerAlreadyRunning(rname))
        throw std::runtime_error("Server::createRequestHandler(): request handler '" + rname +
                                 "' already running on network");

      // first insert nullptr, then create request handler
      auto inserted = m_requestHandlerMap.insert(RequestHandlerMap::value_type(rname, nullptr));

      if (inserted.second) {
        RequestHandler *pRequestHandler = new RequestHandler(this, rname, pController, function);
        inserted.first->second = pRequestHandler;

        if (this->isRunning())
          pRequestHandler->startHandlingRequest();
      } else
        throw;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Server::createCommandHandler(const std::string &cname, Controller *pController,
                                             void (Controller::*function)(const Buffer &command)) {
      auto findIter = m_commandHandlerMap.find(cname);

      if (findIter != m_commandHandlerMap.end()) {
        findIter->second->onCommand().connect(pController, function);

        if (this->isRunning())
          findIter->second->startHandlingCommands();

        return;
      }

      if (Server::commandHandlerAlreadyRunning(cname))
        throw std::runtime_error("Server::createCommandHandler(): command handler '" + cname +
                                 "' already running on network");

      // first insert nullptr, then create command handler
      auto inserted = m_commandHandlerMap.insert(CommandHandlerMap::value_type(cname, nullptr));

      if (inserted.second) {
        CommandHandler *pCommandHandler = new CommandHandler(this, cname, pController, function);
        inserted.first->second = pCommandHandler;

        if (this->isRunning())
          pCommandHandler->startHandlingCommands();
      } else
        throw;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Server::stopCommandHandler(const std::string &cname, Controller *pController) {
      auto findIter = m_commandHandlerMap.find(cname);

      if (findIter != m_commandHandlerMap.end()) {
        findIter->second->onCommand().disconnect(pController);

        if (!findIter->second->onCommand().hasConnection())
          m_commandHandlerMap.erase(findIter);
      }
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline void Server::stopCommandHandler(const std::string &cname, Controller *pController,
                                           void (Controller::*function)(const Buffer &command)) {
      auto findIter = m_commandHandlerMap.find(cname);

      if (findIter != m_commandHandlerMap.end()) {
        findIter->second->onCommand().disconnect(pController, function);

        if (!findIter->second->onCommand().hasConnection())
          m_commandHandlerMap.erase(findIter);
      }
    }

  }
  
}

#endif //  SERVER_H
