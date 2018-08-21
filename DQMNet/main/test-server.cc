//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dqm4hep/Server.h"
#include "dqm4hep/Service.h"

using namespace dqm4hep::net;

class MyPrintClass {
public:
  void print(const Buffer &request, Buffer &response) {
    std::string requestStr(request.begin(), request.size());
    std::cout << "Received : " << requestStr << std::endl;

    std::string responseStr = "Hello world !";
    std::cout << "Sending : " << responseStr << std::endl;

    auto model = response.createModel<std::string>();
    model->move(std::move(responseStr));
    response.setModel(model);
  }

  void printCommand(const Buffer &command) {
    std::string commandStr(command.begin(), command.size());
    std::cout << "Received command : " << commandStr << std::endl;
  }
};

int main(int /*argc*/, char ** /*argv*/) {
  MyPrintClass printer;

  Server *pServer = new Server("TestServer");

  Service *pIntService = pServer->createService("/test/int");
  Service *pFloatService = pServer->createService("/test/float");
  pServer->createRequestHandler("/test/print", &printer, &MyPrintClass::print);
  pServer->createCommandHandler("/test/printCommand", &printer, &MyPrintClass::printCommand);

  pServer->start();

  int intVal = rand();
  float floatVal = intVal * 0.78;

  while (1) {
    intVal = rand();
    std::cout << "Sending int = " << intVal << std::endl;
    pIntService->send(intVal);

    floatVal = intVal * 0.78;
    std::cout << "Sending float = " << floatVal << std::endl;
    pFloatService->send(floatVal);

    sleep(5);
  }

  return 0;
}
