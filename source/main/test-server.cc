  /// \file test-server.cc
/*
 *
 * test-server.cc source template automatically generated by a class generator
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


#include "dqm4hep/Server.h"
#include "dqm4hep/Service.h"

using namespace dqm4hep::net;

class MyPrintClass
{
public:
  void print(const Buffer &request, Buffer &response)
  {
    std::string requestStr(request.begin(), request.size());
    std::string responseStr = "Hello world !";
    response.adopt(responseStr.c_str(), responseStr.size());
    std::cout << "Received : " << requestStr << std::endl;
    std::cout << "Sending : " << responseStr << std::endl;
  }

  void printCommand(const Buffer &command)
  {
    std::string commandStr(command.begin(), command.size());
    std::cout << "Received command : " << commandStr << std::endl;
  }
};

int main(int argc, char **argv)
{
  MyPrintClass printer;

  Server *pServer = new Server("TestServer");

  Service *pIntService   = pServer->createService("/test/int");
  Service *pFloatService = pServer->createService("/test/float");
  pServer->createRequestHandler("/test/print", &printer, &MyPrintClass::print);
  pServer->createCommandHandler("/test/printCommand", &printer, &MyPrintClass::printCommand);

  pServer->start();

  int intVal = rand();
  float floatVal = intVal*0.78;

  while(1)
  {
    intVal = rand();
    std::cout << "Sending int = " << intVal << std::endl;
    pIntService->send(intVal);

    floatVal = intVal*0.78;
    std::cout << "Sending float = " << floatVal << std::endl;
    pFloatService->send(floatVal);

    sleep(5);
  }

  return 0;
}
