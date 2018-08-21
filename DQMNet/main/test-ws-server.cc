//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dqm4hep/WebSocketServer.h"

#include <signal.h>

using namespace dqm4hep::net;

std::shared_ptr<WsServer> aServer = nullptr;

void int_key_signal_handler(int) {
  if(aServer) {
    aServer->stop();
    exit(0);
  }
}

class MyHandler {
public:
  void handleRequest(const WsConnection &con, const WsMessage &request, WsMessage &response) {
    std::cout << "Received request from client " << con.get() << std::endl;
    std::cout << "Request message: " << request->get_payload() << std::endl;
    std::string responseStr = "Hello world" + dqm4hep::core::typeToString(rand());
    response->set_payload(responseStr);
    std::cout << "Sending back: " << response->get_payload() << std::endl;
  }

  void handleCommand(const WsConnection &con, const WsMessage &command) {
    std::cout << "Received command from client " << con.get() << std::endl;
    std::cout << "Command message: " << command->get_payload() << std::endl;
  }
};

int main(int argc, char **argv) {
  
  int port = 8080;
  if(argc > 1) {
    port = atoi(argv[1]);
  }
  
  MyHandler handler;
  aServer = std::make_shared<WsServer>();
  aServer->setPort(port);

  auto intService = aServer->createService("/test/int");
  auto floatService = aServer->createService("/test/float");
  auto requestHandler = aServer->createRequestHandler("/test/request");
  requestHandler->onRequest().connect(&handler, &MyHandler::handleRequest);
  auto commandHandler = aServer->createCommandHandler("/test/command");
  commandHandler->onCommand().connect(&handler, &MyHandler::handleCommand);

  aServer->start();
  
  signal(SIGINT,  int_key_signal_handler);

  int intVal = rand();
  float floatVal = intVal * 0.78;

  while (1) {
    intVal = rand();
    // std::cout << "Sending int = " << intVal << std::endl;
    intService->send(intVal);

    floatVal = intVal * 0.78;
    // std::cout << "Sending float = " << floatVal << std::endl;
    floatService->send(floatVal);

    dqm4hep::core::time::msleep(100);
  }

  return 0;
}
