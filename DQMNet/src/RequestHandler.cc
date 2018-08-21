//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dqm4hep/RequestHandler.h"

namespace dqm4hep {

  namespace net {

    RequestHandler::~RequestHandler() {
      this->stopHandlingRequest();
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &RequestHandler::name() const {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    Server *RequestHandler::server() const {
      return m_pServer;
    }

    //-------------------------------------------------------------------------------------------------

    void RequestHandler::startHandlingRequest() {
      if (!this->isHandlingRequest()) {
        m_pRpc = new Rpc(this);
      }
    }

    //-------------------------------------------------------------------------------------------------

    void RequestHandler::stopHandlingRequest() {
      if (this->isHandlingRequest()) {
        delete m_pRpc;
        m_pRpc = nullptr;
      }
    }

    //-------------------------------------------------------------------------------------------------

    bool RequestHandler::isHandlingRequest() const {
      return (m_pRpc != nullptr);
    }

    //-------------------------------------------------------------------------------------------------

    RequestHandler::RequestSignal &RequestHandler::onRequest() {
      return m_requestSignal;
    }

    //-------------------------------------------------------------------------------------------------

    void RequestHandler::handleRequest(const Buffer &request, Buffer &response) {
      m_requestSignal.process(request, response);
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    RequestHandler::Rpc::Rpc(RequestHandler *pHandler)
        : DimRpc((char *)pHandler->name().c_str(), "C", "C"), m_pHandler(pHandler) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    void RequestHandler::Rpc::rpcHandler() {
      char *data = (char *)this->getData();
      int size = this->getSize();
      Buffer request;

      if (nullptr != data && size != 0)
        request.adopt(data, size);

      Buffer response;
      m_pHandler->handleRequest(request, response);
      this->setData((void *)response.begin(), response.size());
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    CommandHandler::CommandHandler(Server *pServer, const std::string &cname)
        : m_name(cname), m_pServer(pServer), m_pCommand(nullptr) {
    }

    //-------------------------------------------------------------------------------------------------

    CommandHandler::~CommandHandler() {
      this->stopHandlingCommands();
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &CommandHandler::name() const {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    Server *CommandHandler::server() const {
      return m_pServer;
    }

    //-------------------------------------------------------------------------------------------------

    void CommandHandler::startHandlingCommands() {
      if (!this->isHandlingCommands()) {
        m_pCommand = new Command(this);
      }
    }

    //-------------------------------------------------------------------------------------------------

    void CommandHandler::stopHandlingCommands() {
      if (this->isHandlingCommands()) {
        delete m_pCommand;
        m_pCommand = nullptr;
      }
    }

    //-------------------------------------------------------------------------------------------------

    bool CommandHandler::isHandlingCommands() const {
      return (m_pCommand != nullptr);
    }

    //-------------------------------------------------------------------------------------------------

    CommandHandler::CommandSignal &CommandHandler::onCommand() {
      return m_commandSignal;
    }

    //-------------------------------------------------------------------------------------------------

    void CommandHandler::handleCommand(const Buffer &command) {
      m_commandSignal.process(command);
    }

    //------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------

    CommandHandler::Command::Command(CommandHandler *pHandler)
        : DimCommand((char *)pHandler->name().c_str(), "C"), m_pHandler(pHandler) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    void CommandHandler::Command::commandHandler() {
      char *data = (char *)this->getData();
      int size = this->getSize();

      if (nullptr == data || size == 0)
        return;

      Buffer command;
      command.adopt(data, size);
      m_pHandler->handleCommand(command);
    }
  }
}
