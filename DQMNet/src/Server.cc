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
#include <dqm4hep/Internal.h>
#include <dqm4hep/Server.h>
#include <dqm4hep/Logging.h>

// -- std headers
#include <sys/utsname.h>
#include <unistd.h>

// -- dim headers
#include <dic.hxx>

namespace dqm4hep {

  namespace net {

    Server::Server(const std::string &sname)
        : m_name(sname),
          m_started(false) {
      DimServer::addClientExitHandler(this);
      m_serverInfoHandler = new RequestHandler(this, "/" + m_name + "/info", this, &Server::handleServerInfoRequest);
    }

    //-------------------------------------------------------------------------------------------------

    Server::~Server() {
      this->stop();
      this->clear();
      delete m_serverInfoHandler;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &Server::name() const {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    void Server::start() {
      if (m_started)
        return;

      for (auto iter = m_serviceMap.begin(), endIter = m_serviceMap.end(); endIter != iter; ++iter) {
        if (!iter->second->isServiceConnected())
          iter->second->connectService();
      }

      for (auto iter = m_requestHandlerMap.begin(), endIter = m_requestHandlerMap.end(); endIter != iter; ++iter) {
        if (!iter->second->isHandlingRequest())
          iter->second->startHandlingRequest();
      }

      for (auto iter = m_commandHandlerMap.begin(), endIter = m_commandHandlerMap.end(); endIter != iter; ++iter) {
        if (!iter->second->isHandlingCommands())
          iter->second->startHandlingCommands();
      }

      if (!m_serverInfoHandler->isHandlingRequest())
        m_serverInfoHandler->startHandlingRequest();

      DimServer::start(const_cast<char *>(m_name.c_str()));

      m_started = true;
    }

    //-------------------------------------------------------------------------------------------------

    void Server::stop() {
      if (!m_started)
        return;

      for (auto iter = m_serviceMap.begin(), endIter = m_serviceMap.end(); endIter != iter; ++iter) {
        if (iter->second->isServiceConnected())
          iter->second->disconnectService();
      }

      for (auto iter = m_requestHandlerMap.begin(), endIter = m_requestHandlerMap.end(); endIter != iter; ++iter) {
        if (iter->second->isHandlingRequest())
          iter->second->stopHandlingRequest();
      }

      for (auto iter = m_commandHandlerMap.begin(), endIter = m_commandHandlerMap.end(); endIter != iter; ++iter) {
        if (iter->second->isHandlingCommands())
          iter->second->stopHandlingCommands();
      }

      if (m_serverInfoHandler->isHandlingRequest())
        m_serverInfoHandler->stopHandlingRequest();

      DimServer::stop();
      m_started = false;
    }

    //-------------------------------------------------------------------------------------------------

    bool Server::isRunning() const {
      return m_started;
    }

    //-------------------------------------------------------------------------------------------------

    void Server::clear() {
      for (auto iter = m_serviceMap.begin(), endIter = m_serviceMap.end(); endIter != iter; ++iter)
        delete iter->second;

      for (auto iter = m_requestHandlerMap.begin(), endIter = m_requestHandlerMap.end(); endIter != iter; ++iter)
        delete iter->second;

      for (auto iter = m_commandHandlerMap.begin(), endIter = m_commandHandlerMap.end(); endIter != iter; ++iter)
        delete iter->second;

      m_serviceMap.clear();
      m_requestHandlerMap.clear();
      m_commandHandlerMap.clear();
      DimServer::stop();
      m_started = false;
    }

    //-------------------------------------------------------------------------------------------------

    Service *Server::createService(const std::string &sname) {
      if (sname.empty())
        throw std::runtime_error("Server::createService(): service name is invalid");

      auto findIter = m_serviceMap.find(sname);

      if (findIter != m_serviceMap.end())
        return findIter->second;

      if (Server::serviceAlreadyRunning(sname))
        throw std::runtime_error("Server::createService(): service '" + sname + "' already running on network");

      // first insert nullptr, then create the service
      std::pair<ServiceMap::iterator, bool> inserted = m_serviceMap.insert(ServiceMap::value_type(sname, nullptr));

      if (inserted.second) {
        Service *pService = new Service(this, sname);
        inserted.first->second = pService;

        if (this->isRunning())
          pService->connectService();

        return pService;
      } else
        throw;
    }

    //-------------------------------------------------------------------------------------------------

    bool Server::isServiceRegistered(const std::string &sname) const {
      return (m_serviceMap.find(sname) != m_serviceMap.end());
    }

    //-------------------------------------------------------------------------------------------------

    bool Server::isRequestHandlerRegistered(const std::string &rname) const {
      return (m_requestHandlerMap.find(rname) != m_requestHandlerMap.end());
    }

    //-------------------------------------------------------------------------------------------------

    bool Server::isCommandHandlerRegistered(const std::string &cname) const {
      return (m_commandHandlerMap.find(cname) != m_commandHandlerMap.end());
    }

    //-------------------------------------------------------------------------------------------------

    void Server::startService(const std::string &sname) {
      Service *pService = this->service(sname);

      if (nullptr != pService && !pService->isServiceConnected())
        pService->connectService();
    }

    //-------------------------------------------------------------------------------------------------

    void Server::stopService(const std::string &sname) {
      Service *pService = this->service(sname);

      if (nullptr != pService && pService->isServiceConnected())
        pService->disconnectService();
    }

    //-------------------------------------------------------------------------------------------------

    void Server::startRequestHandler(const std::string &rname) {
      RequestHandler *pRequestHandler = this->requestHandler(rname);

      if (nullptr != pRequestHandler && !pRequestHandler->isHandlingRequest())
        pRequestHandler->startHandlingRequest();
    }

    //-------------------------------------------------------------------------------------------------

    void Server::stopRequestHandler(const std::string &rname) {
      RequestHandler *pRequestHandler = this->requestHandler(rname);

      if (nullptr != pRequestHandler && pRequestHandler->isHandlingRequest())
        pRequestHandler->stopHandlingRequest();
    }

    //-------------------------------------------------------------------------------------------------

    void Server::startCommandHandler(const std::string &cname) {
      CommandHandler *pCommandHandler = this->commandHandler(cname);

      if (nullptr != pCommandHandler && !pCommandHandler->isHandlingCommands())
        pCommandHandler->startHandlingCommands();
    }

    //-------------------------------------------------------------------------------------------------

    void Server::stopCommandHandler(const std::string &cname) {
      CommandHandler *pCommandHandler = this->commandHandler(cname);

      if (nullptr != pCommandHandler && pCommandHandler->isHandlingCommands())
        pCommandHandler->stopHandlingCommands();
    }

    //-------------------------------------------------------------------------------------------------

    Service *Server::service(const std::string &sname) const {
      auto findIter = m_serviceMap.find(sname);
      return (findIter == m_serviceMap.end() ? nullptr : findIter->second);
    }

    //-------------------------------------------------------------------------------------------------

    RequestHandler *Server::requestHandler(const std::string &rname) const {
      auto findIter = m_requestHandlerMap.find(rname);
      return (findIter == m_requestHandlerMap.end() ? nullptr : findIter->second);
    }

    //-------------------------------------------------------------------------------------------------

    CommandHandler *Server::commandHandler(const std::string &cname) const {
      auto findIter = m_commandHandlerMap.find(cname);
      return (findIter == m_commandHandlerMap.end() ? nullptr : findIter->second);
    }

    //-------------------------------------------------------------------------------------------------

    core::Signal<int> &Server::onClientExit() {
      return m_clientExitSignal;
    }

    //-------------------------------------------------------------------------------------------------

    int Server::clientId() const {
      return DimServer::getClientId();
    }

    //-------------------------------------------------------------------------------------------------

    std::string Server::dnsNode() {
      char *pDnsNode = DimServer::getDnsNode();

      if (pDnsNode)
        return pDnsNode;

      pDnsNode = getenv("DIM_DNS_NODE");
      return (pDnsNode ? pDnsNode : "");
    }

    //-------------------------------------------------------------------------------------------------

    int Server::dnsPort() {
      return DimServer::getDnsPort();
    }

    //-------------------------------------------------------------------------------------------------

    std::vector<std::string> Server::runningServers() {
      std::vector<std::string> runningServers;

      DimBrowser browser;
      browser.getServers();
      char *pServer, *pNode;

      while (browser.getNextServer(pServer, pNode)) {
        std::string server(pServer);
        runningServers.push_back(server);
      }

      return runningServers;
    }

    //-------------------------------------------------------------------------------------------------

    bool Server::isServerRunning(const std::string &serverName) {
      DimBrowser browser;
      browser.getServers();
      char *pServer, *pNode;

      while (browser.getNextServer(pServer, pNode)) {
        std::string server(pServer);

        if (server == serverName)
          return true;
      }

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    void Server::handleServerInfoRequest(const Buffer & /*request*/, Buffer &response) {
      // get the list of services, request handlers and command handlers
      core::StringVector serviceList, requestHandlerList, commandHandlerList;

      for (auto pservice : m_serviceMap) {
        serviceList.push_back(pservice.second->name());
      }

      for (auto request : m_requestHandlerMap) {
        requestHandlerList.push_back(request.second->name());
      }

      for (auto command : m_commandHandlerMap) {
        commandHandlerList.push_back(command.second->name());
      }

      // get host info
      core::StringMap hostInfo;
      core::fillHostInfo(hostInfo);

      core::json jsonResponse = {{"server", {{"name", m_name}}},
                                 {"host", hostInfo},
                                 {"services", serviceList},
                                 {"requestHandlers", requestHandlerList},
                                 {"commandHandlers", commandHandlerList}};

      std::string serializedJson = jsonResponse.dump();
      auto model = response.createModel<std::string>();
      response.setModel(model);
      model->move(std::move(serializedJson));
    }

    //-------------------------------------------------------------------------------------------------

    bool Server::serviceAlreadyRunning(const std::string &sname) {
      if(DimServer::inCallback()) {
        dqm_warning( "Server::serviceAlreadyRunning: can't check for duplicated service on network !" );
        return false;
      }
      DimBrowser browser;
      int nServices = browser.getServices(sname.c_str());

      if (nServices == 0)
        return false;

      int serviceType;
      char *serviceName, *format;

      while (1) {
        serviceType = browser.getNextService(serviceName, format);

        if (serviceType == 0)
          break;

        if (serviceType == DimSERVICE)
          return true;
      }

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    bool Server::requestHandlerAlreadyRunning(const std::string &rname) {
      if(DimServer::inCallback()) {
        dqm_warning( "Server::requestHandlerAlreadyRunning: can't check for duplicated request handler on network !" );
        return false;
      }
      DimBrowser browser;
      int nServices = browser.getServices(rname.c_str());

      if (nServices == 0)
        return false;

      int serviceType;
      char *serviceName, *format;

      while (1) {
        serviceType = browser.getNextService(serviceName, format);

        if (serviceType == 0)
          break;

        if (serviceType == DimRPC)
          return true;
      }

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    bool Server::commandHandlerAlreadyRunning(const std::string &cname) {
      if(DimServer::inCallback()) {
        dqm_warning( "Server::commandHandlerAlreadyRunning: can't check for duplicated command on network !" );
        return false;
      }
      DimBrowser browser;
      int nServices = browser.getServices(cname.c_str());

      if (nServices == 0)
        return false;

      int serviceType;
      char *serviceName, *format;

      while (1) {
        serviceType = browser.getNextService(serviceName, format);

        if (serviceType == 0)
          break;

        if (serviceType == DimCOMMAND)
          return true;
      }

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    void Server::clientExitHandler() {
      int clientID(DimServer::getClientId());
      std::cout << "Client " << clientID << " exits" << std::endl;
      m_clientExitSignal.process(clientID);
    }

  }

}
