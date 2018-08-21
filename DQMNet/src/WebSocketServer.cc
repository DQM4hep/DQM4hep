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
#include <dqm4hep/WebSocketServer.h>
#include <dqm4hep/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;

namespace dqm4hep {

  namespace net {
    
    WsServiceBase::WsServiceBase(WsServer *s, const std::string &n, ServiceType t) :
      m_server(s),
      m_name(n),
      m_type(t) {
      /* nop */    
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsServiceBase::~WsServiceBase() {
      /* nop */
    }
    
    //-------------------------------------------------------------------------------------------------
      
    WsServer* WsServiceBase::server() const {
      return m_server;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const std::string &WsServiceBase::name() const {
      return m_name;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    ServiceType WsServiceBase::type() const {
      return m_type;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsService::WsService(WsServer *s, const std::string &n) :
      WsServiceBase(s, n, SERVICE_TYPE) {
        
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsService::send(const char *buffer, size_t size, bool containsBinary) {
      server()->send(this, buffer, size, containsBinary);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    WsCommandHandler::WsCommandHandler(WsServer *s, const std::string &n) :
      WsServiceBase(s, n, COMMAND_TYPE) {
      /* nop */
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsCommandHandler::signal_type &WsCommandHandler::onCommand() {
      return m_signal;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    WsRequestHandler::WsRequestHandler(WsServer *s, const std::string &n) :
      WsServiceBase(s, n, RPC_TYPE) {
      
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsRequestHandler::signal_type &WsRequestHandler::onRequest() {
      return m_signal;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    WsServer::WsServer() :
      m_server() {
      
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsServer::~WsServer() {
      stop();
      for(auto &svc : m_serviceMap) {
        delete svc.second;
      }
      m_serviceConnections.clear();
      m_serviceMap.clear();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::setPort(int port) {
      if(not m_running.load()) {
        m_port = port;        
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsService *WsServer::createService(const std::string &name) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto findIter = m_serviceMap.find(name);
      if(findIter != m_serviceMap.end()) {
        dqm_error("Couldn't create service '{0}' twice", name);
        return nullptr; 
      }
      WsService *service = new WsService(this, name);
      m_serviceMap.insert(ServiceMap::value_type(name, service));
      m_serviceConnections[name] = connection_set();
      return service;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsCommandHandler *WsServer::createCommandHandler(const std::string &name) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto findIter = m_serviceMap.find(name);
      if(findIter != m_serviceMap.end()) {
        dqm_error("Couldn't create service '{0}' twice", name);
        return nullptr; 
      }
      WsCommandHandler *service = new WsCommandHandler(this, name);
      m_serviceMap.insert(ServiceMap::value_type(name, service));
      return service;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsRequestHandler *WsServer::createRequestHandler(const std::string &name) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto findIter = m_serviceMap.find(name);
      if(findIter != m_serviceMap.end()) {
        dqm_error("Couldn't create service '{0}' twice", name);
        return nullptr; 
      }
      WsRequestHandler *service = new WsRequestHandler(this, name);
      m_serviceMap.insert(ServiceMap::value_type(name, service));
      return service;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsService *WsServer::findService(const std::string &name) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto findIter = m_serviceMap.find(name);
      if(findIter == m_serviceMap.end()) {
        return nullptr;
      }
      if(findIter->second->type() != SERVICE_TYPE) {
        return nullptr;
      }
      return dynamic_cast<WsService*>(findIter->second);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsCommandHandler *WsServer::findCommandHandler(const std::string &name) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto findIter = m_serviceMap.find(name);
      if(findIter == m_serviceMap.end()) {
        return nullptr;
      }
      if(findIter->second->type() != COMMAND_TYPE) {
        return nullptr;
      }
      return dynamic_cast<WsCommandHandler*>(findIter->second);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    WsRequestHandler *WsServer::findRequestHandler(const std::string &name) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto findIter = m_serviceMap.find(name);
      if(findIter == m_serviceMap.end()) {
        return nullptr;
      }
      if(findIter->second->type() != RPC_TYPE) {
        return nullptr;
      }
      return dynamic_cast<WsRequestHandler*>(findIter->second);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::start() {
      if(m_running.load()) {
        return;
      }   
      // Set logging settings
      m_server.set_access_channels(websocketpp::log::alevel::none);

      // Initialize Asio
      m_server.init_asio();

      // Register our message handler
      m_server.set_message_handler(std::bind(&WsServer::onMessage, this, _1, _2));
      m_server.set_open_handler(std::bind(&WsServer::onOpen, this, _1));
      m_server.set_close_handler(std::bind(&WsServer::onClose, this, _1));

      // Start listening
      m_server.listen(m_port);

      // Start the server accept loop
      m_server.start_accept();

      // Start the ASIO io_service run loop
      m_thread = std::thread(&server::run, std::ref(m_server));
      m_running = true;
    }
    
    //-------------------------------------------------------------------------------------------------

    void WsServer::stop() {
      if(m_running.load()) {
        // std::lock_guard<std::recursive_mutex> lock(m_mutex);
        m_server.stop();
        m_thread.join();
        m_serviceConnections.clear();
        m_running = false; 
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::onOpen(connection_hdl /*hdl*/) {
      
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::onClose(connection_hdl hdl) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      server::connection_ptr con = m_server.get_con_from_hdl(hdl);
      const std::string serviceName = con->get_resource();
      auto findIter = m_serviceMap.find(serviceName);
      if(m_serviceMap.end() == findIter) {
        return;
      }
      // remove service subscriber (if subscribed)
      if(findIter->second->type() == SERVICE_TYPE) {
        auto findIter2 = m_serviceConnections.find(serviceName);
        if(findIter2 == m_serviceConnections.end()) {
          return;            
        }
        findIter2->second.erase(con);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::onMessage(connection_hdl hdl, message_ptr msg) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      server::connection_ptr con = m_server.get_con_from_hdl(hdl);
      const std::string serviceName = con->get_resource();
      auto findIter = m_serviceMap.find(serviceName);
      
      if(findIter == m_serviceMap.end()) {
        m_server.close(hdl, websocketpp::close::status::normal, "Service '" + serviceName + "' not available !");
        return;
      }
      // handle service subscription / unsubscription
      if(findIter->second->type() == SERVICE_TYPE) {
        onServiceMessage(serviceName, hdl, msg);
      }
      else if(findIter->second->type() == COMMAND_TYPE) {
        onCommandMessage(serviceName, hdl, msg);
      }
      else if(findIter->second->type() == RPC_TYPE) {
        onRpcMessage(serviceName, hdl, msg);
      }
      else {
        m_server.close(hdl, websocketpp::close::status::normal, "Service '" + serviceName + "' unknown type !");
        return;
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::send(WsService *service, const char *buffer, size_t size, bool containsBinary) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto findIter = m_serviceConnections.find(service->name());
      if(findIter == m_serviceConnections.end()) {
        return;
      }
      websocketpp::frame::opcode::value opcode = containsBinary ? websocketpp::frame::opcode::binary : websocketpp::frame::opcode::text;
      for(auto &con : findIter->second) {
        m_server.send(con, buffer, size, opcode);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::onServiceMessage(const std::string &serviceName, connection_hdl hdl, message_ptr msg) {
      server::connection_ptr con = m_server.get_con_from_hdl(hdl);
      auto findIter2 = m_serviceConnections.find(serviceName);
      // handle subscription/un-subscription to/from service
      if(msg->get_payload() == "subscribe") {
        if(findIter2 == m_serviceConnections.end()) {
          m_server.close(hdl, websocketpp::close::status::normal, "Internal error: service '" + serviceName + "' not available !");
          return;
        }
        // insert new subscriber
        findIter2->second.insert(con);
        m_server.send(hdl, "ok", websocketpp::frame::opcode::text);
        return;
      }
      else if(msg->get_payload() == "unsubscribe") {
        if(findIter2 == m_serviceConnections.end()) {
          m_server.close(hdl, websocketpp::close::status::normal, "Internal error: service '" + serviceName + "' not available !");
          return;
        }
        // remove subscriber
        findIter2->second.erase(con);
        m_server.send(hdl, "ok", websocketpp::frame::opcode::text);
        return;
      }
      else {
        m_server.close(hdl, websocketpp::close::status::normal, "Unknown command for service '" + serviceName + "', command: " + msg->get_payload() + " !");
        return;            
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::onCommandMessage(const std::string &serviceName, connection_hdl hdl, message_ptr msg) {
      WsCommandHandler *command = findCommandHandler(serviceName);
      if(nullptr == command) {
        m_server.close(hdl, websocketpp::close::status::normal, "Internal error: command '" + serviceName + "' not available !");
        return;
      }
      server::connection_ptr con = m_server.get_con_from_hdl(hdl);
      command->onCommand().emit(con, msg);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void WsServer::onRpcMessage(const std::string &serviceName, connection_hdl hdl, message_ptr msg) {
      WsRequestHandler *request = findRequestHandler(serviceName);
      if(nullptr == request) {
        m_server.close(hdl, websocketpp::close::status::normal, "Internal error: request '" + serviceName + "' not available !");
        return;
      }
      server::connection_ptr con = m_server.get_con_from_hdl(hdl);
      message_ptr responseMsg = con->get_message(websocketpp::frame::opcode::text, 0);
      request->onRequest().emit(con, msg, responseMsg);
      m_server.send(con, responseMsg);
    }
    
  }

}
