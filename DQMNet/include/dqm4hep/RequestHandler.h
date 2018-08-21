//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/NetBuffer.h"
#include "dqm4hep/Signal.h"
#include "dqm4hep/json.h"

// -- std headers
#include <string>

// -- dim headers
#include "dis.hxx"

namespace dqm4hep {

  namespace net {

    class Server;

    class RequestHandler {
      friend class Server;

    public:
      typedef core::Signal<const Buffer &, Buffer &> RequestSignal;

      /**
       * Get the request name
       */
      const std::string &name() const;

      /**
       * Get the server in which the request handler is declared
       */
      Server *server() const;

    private:
      /**
       * Constructor
       *
       * @param pServer the server managing the request handler
       * @param name the request handler name
       */
      template <typename Controller>
      RequestHandler(Server *pServer, const std::string &name, Controller *pController,
                     void (Controller::*function)(const Buffer &request, Buffer &response));
      
      RequestHandler(const RequestHandler&) = delete;
      RequestHandler& operator=(const RequestHandler&) = delete;

      /**
       * Destructor
       */
      ~RequestHandler();

      /**
       * Create the actual request handler connection
       */
      void startHandlingRequest();

      /**
       * Remove the actual request handler connection
       */
      void stopHandlingRequest();

      /**
       * Whether the request handler is connected
       */
      bool isHandlingRequest() const;

      /**
       * [onRequest description]
       * @return [description]
       */
      RequestSignal &onRequest();

    private:
      /** Rpc class.
      *
      *  The concrete dim rpc implementation
      */
      class Rpc : public DimRpc {
      public:
        /**
         * Contructor
         */
        Rpc(RequestHandler *pHandler);
        Rpc(const Rpc&) = delete;
        Rpc& operator=(const Rpc&) = delete;

        /**
         * The dim rpc handler
         */
        void rpcHandler() override;

      private:
        RequestHandler *m_pHandler = {nullptr}; ///< The request handler owner instance
      };

      friend class Rpc;

    private:
      /**
       * [handleRequest description]
       * @param request  [description]
       * @param response [description]
       */
      void handleRequest(const Buffer &request, Buffer &response);

    private:
      std::string           m_name = {""};            ///< The request handler name
      Server               *m_pServer = {nullptr};         ///< The server in which the request handler is declared
      RequestSignal         m_requestSignal = {};
      Rpc                  *m_pRpc = {nullptr};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline RequestHandler::RequestHandler(Server *pServer, const std::string &rname, Controller *pController,
                                          void (Controller::*function)(const Buffer &request, Buffer &response))
        : m_name(rname), m_pServer(pServer), m_pRpc(nullptr) {
      m_requestSignal.connect(pController, function);
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    class CommandHandler {
      friend class Server;

    public:
      typedef core::Signal<const Buffer &> CommandSignal;

      /**
       * Get the request name
       */
      const std::string &name() const;

      /**
       * Get the server in which the request handler is declared
       */
      Server *server() const;

    private:
      /**
       * Constructor
       *
       * @param pServer the server managing the request handler
       * @param name the request handler name
       */
      template <typename Controller>
      CommandHandler(Server *pServer, const std::string &name, Controller *pController,
                     void (Controller::*function)(const Buffer &command));

      CommandHandler(const CommandHandler&) = delete;
      CommandHandler& operator=(const CommandHandler&) = delete;

      /**
       * Constructor
       *
       * @param pServer the server managing the request handler
       * @param name the request handler name
       */
      CommandHandler(Server *pServer, const std::string &name);

      /**
       * Destructor
       */
      ~CommandHandler();

      /**
       * Create the actual request handler connection
       */
      void startHandlingCommands();

      /**
       * Remove the actual request handler connection
       */
      void stopHandlingCommands();

      /**
       * Whether the request handler is connected
       */
      bool isHandlingCommands() const;

      /**
       * [onRequest description]
       * @return [description]
       */
      CommandSignal &onCommand();

    private:
      /** Command class.
      *
      *  The concrete dim command implementation
      */
      class Command : public DimCommand {
      public:
        /**
         * Contructor
         */
        Command(CommandHandler *pHandler);
        Command(const Command&) = delete;
        Command& operator=(const Command&) = delete;

        /**
         * The dim command handler
         */
        void commandHandler() override;

      private:
        CommandHandler *m_pHandler = {nullptr}; ///< The request handler owner instance
      };

      friend class Command;

    private:
      /**
       * [handleCommand description]
       * @param request  [description]
       * @param response [description]
       */
      void handleCommand(const Buffer &command);

    private:
      std::string         m_name = {""};           ///< The command handler name
      Server             *m_pServer = {nullptr};   ///< The server in which the command handler is declared
      CommandSignal       m_commandSignal = {};
      Command            *m_pCommand = {nullptr};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename Controller>
    inline CommandHandler::CommandHandler(Server *pServer, const std::string &cname, Controller *pController,
                                          void (Controller::*function)(const Buffer &command))
        : m_name(cname), m_pServer(pServer) {
      m_commandSignal.connect(pController, function);
    }
  }
}

#endif //  REQUESTHANDLER_H
