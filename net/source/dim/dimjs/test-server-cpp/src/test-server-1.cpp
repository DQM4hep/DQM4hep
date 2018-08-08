#include "server_ws.hpp"
#include "dic.hxx"
#include "json/json.h"

using namespace std;

typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;

enum DimType {
  INT = 0,
  FLOAT,
  DOUBLE,
  SHORT,
  LONGLONG,
  STRING,
  BINARY
};


int main() {

    WsServer server;
    server.config.port=2511;

    ServiceMgr serviceMgr(&server);

    auto& getServices=server.endpoint["^/browser/getServices/?$"];
    auto& getServers=server.endpoint["^/browser/getServers/?$"];
    auto& getServerServices=server.endpoint["^/browser/getServerServices/?$"];
    auto& getServerClients=server.endpoint["^/browser/getServerClients/?$"];

    getServices.on_message=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {
        auto message_str=message->string();
        cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << endl;

        DimBrowser browser;
        int nServices = browser.getServices(message_str.c_str());

        char *service, *format; int type;
        Json::Value jsonStream, servicesValue(Json::arrayValue);
        unsigned int index = 0;

        while(type = browser.getNextService(service, format))
        {
          Json::Value serviceValue;
          serviceValue["service"] = service;
          serviceValue["format"] = format;
          serviceValue["type"] = type;

          servicesValue[index] = serviceValue;
          index ++;
        }

        jsonStream["services"] = servicesValue;
        Json::FastWriter writer;
        std::string ret = writer.write(jsonStream);

        cout << "Server: Sending message \"" << ret <<  "\" to " << (size_t)connection.get() << endl;

        auto send_stream=make_shared<WsServer::SendStream>();
        *send_stream << ret;

        server.send(connection, send_stream, [](const boost::system::error_code& ec){
            if(ec) {
                cout << "Server: Error sending message. " <<
                        "Error: " << ec << ", error message: " << ec.message() << endl;
            }
        });
    };


    getServers.on_message=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {
        DimBrowser browser;
        browser.getServers();

        char *srv, *node; int pid;
        Json::Value jsonStream, serversValue(Json::arrayValue);
        unsigned int index = 0;

        while(browser.getNextServer(srv, node, pid))
        {
          Json::Value serverValue;
          serverValue["server"] = srv;
          serverValue["node"] = node;
          serverValue["pid"] = pid;

          serversValue[index] = serverValue;
          index ++;
        }

        jsonStream["servers"] = serversValue;
        Json::FastWriter writer;
        std::string ret = writer.write(jsonStream);

        cout << "Server: Sending message \"" << ret <<  "\" to " << (size_t)connection.get() << endl;

        auto send_stream=make_shared<WsServer::SendStream>();
        *send_stream << ret;

        server.send(connection, send_stream, [](const boost::system::error_code& ec){
            if(ec) {
                cout << "Server: Error sending message. " <<
                        "Error: " << ec << ", error message: " << ec.message() << endl;
            }
        });
    };



    getServerServices.on_message=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {

      auto message_str=message->string();
      cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << endl;

        DimBrowser browser;
        browser.getServerServices(message_str.c_str());

        char *service, *format; int type;
        Json::Value jsonStream, servicesValue(Json::arrayValue);
        unsigned int index = 0;

        while(type = browser.getNextServerService(service, format))
        {
          Json::Value serviceValue;
          serviceValue["service"] = service;
          serviceValue["format"] = format;
          serviceValue["type"] = type;

          servicesValue[index] = serviceValue;
          index ++;
        }

        jsonStream["serverServices"] = servicesValue;
        Json::FastWriter writer;
        std::string ret = writer.write(jsonStream);

        cout << "Server: Sending message \"" << ret <<  "\" to " << (size_t)connection.get() << endl;

        auto send_stream=make_shared<WsServer::SendStream>();
        *send_stream << ret;

        server.send(connection, send_stream, [](const boost::system::error_code& ec){
            if(ec) {
                cout << "Server: Error sending message. " <<
                        "Error: " << ec << ", error message: " << ec.message() << endl;
            }
        });
    };

    getServerClients.on_message=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {

      auto message_str=message->string();
      cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << endl;

        DimBrowser browser;
        browser.getServerClients(message_str.c_str());

        char *client, *node;
        Json::Value jsonStream, serversValue(Json::arrayValue);
        unsigned int index = 0;

        while(browser.getNextServerClient(client, node))
        {
          Json::Value serverValue;
          serverValue["client"] = client;
          serverValue["node"] = node;

          serversValue[index] = serverValue;
          index ++;
        }

        jsonStream["serverClients"] = serversValue;
        Json::FastWriter writer;
        std::string ret = writer.write(jsonStream);

        cout << "Server: Sending message \"" << ret <<  "\" to " << (size_t)connection.get() << endl;

        auto send_stream=make_shared<WsServer::SendStream>();
        *send_stream << ret;

        server.send(connection, send_stream, [](const boost::system::error_code& ec){
            if(ec) {
                cout << "Server: Error sending message. " <<
                        "Error: " << ec << ", error message: " << ec.message() << endl;
            }
        });
    };


    auto& command=server.endpoint["^/command/?$"];

    command.on_message=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {


      auto message_str=message->string();
      cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << endl;

      Json::Reader reader;
      Json::Value messageValue;
      bool success = reader.parse(message_str, messageValue);

      if(!success)
        return;

      std::string commandName = messageValue["name"].asString();
      unsigned int type = messageValue["type"].asUInt();

      switch(type)
      {
        case INT:
        {
          int intValue = messageValue["data"].asInt();
          DimClient::sendCommandNB((char *)commandName.c_str(), intValue);
          break;
        }
        case FLOAT:
        {
          float floatValue = messageValue["data"].asFloat();
          DimClient::sendCommandNB((char *)commandName.c_str(), floatValue);
          break;
        }
        case DOUBLE:
        {
          double doubleValue = messageValue["data"].asDouble();
          DimClient::sendCommandNB((char *)commandName.c_str(), doubleValue);
          break;
        }
        case SHORT:
        {
          short shortValue = messageValue["data"].asInt();
          DimClient::sendCommandNB((char *)commandName.c_str(), shortValue);
          break;
        }
        case LONGLONG:
        {
          longlong longlongValue = messageValue["data"].asInt64();
          DimClient::sendCommandNB((char *)commandName.c_str(), longlongValue);
          break;
        }
        case STRING:
        {
          std::string stringValue = messageValue["data"].asString();
          DimClient::sendCommandNB((char *)commandName.c_str(), (char*)stringValue.c_str());
          break;
        }
        default:
          break;
      }
    };


    auto& rpc=server.endpoint["^/rpc/?$"];

    rpc.on_message=[&server](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {


      auto message_str=message->string();
      cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << endl;

      Json::Reader reader;
      Json::Value messageValue;
      bool success = reader.parse(message_str, messageValue);

      if(!success)
        return;

      std::string rpcName = messageValue["name"].asString();
      unsigned int intype = messageValue["intype"].asUInt();
      unsigned int outtype = messageValue["outtype"].asUInt();

      DimBrowser browser;
      std::cout << "rpc '" << rpcName << "' found ? " << (browser.getServices((char*)rpcName.c_str()) ? "yes" : "no") << std::endl;

      DimRpcInfo *rpc=0;

      switch(outtype)
      {
        case INT:
        {
          int intValue = messageValue["nolink"].asInt();
          rpc = new DimRpcInfo((char*)rpcName.c_str(), intValue);
          break;
        }
        case FLOAT:
        {
          float floatValue = messageValue["nolink"].asFloat();
          rpc = new DimRpcInfo((char*)rpcName.c_str(), floatValue);
          break;
        }
        case DOUBLE:
        {
          double doubleValue = messageValue["nolink"].asDouble();
          rpc = new DimRpcInfo((char*)rpcName.c_str(), doubleValue);
          break;
        }
        case SHORT:
        {
          short shortValue = messageValue["nolink"].asInt();
          rpc = new DimRpcInfo((char*)rpcName.c_str(), shortValue);
          break;
        }
        case LONGLONG:
        {
          longlong longlongValue = messageValue["nolink"].asInt64();
          rpc = new DimRpcInfo((char*)rpcName.c_str(), longlongValue);
          break;
        }
        case STRING:
        {
          std::cout << "out is string (1)" << std::endl;
          std::string stringValue = messageValue["nolink"].asString();
          rpc = new DimRpcInfo((char*)rpcName.c_str(), (char*)stringValue.c_str());
          break;
        }
        default:
          break;
      }

      if(!rpc)
        return;

      switch(intype)
      {
        case INT:
        {
          int intValue = messageValue["data"].asInt();
          rpc->setData(intValue);
          break;
        }
        case FLOAT:
        {
          float floatValue = messageValue["data"].asFloat();
          rpc->setData(floatValue);
          break;
        }
        case DOUBLE:
        {
          double doubleValue = messageValue["data"].asDouble();
          rpc->setData(doubleValue);
          break;
        }
        case SHORT:
        {
          short shortValue = messageValue["data"].asInt();
          rpc->setData(shortValue);
          break;
        }
        case LONGLONG:
        {
          longlong longlongValue = messageValue["data"].asInt64();
          rpc->setData(longlongValue);
          break;
        }
        case STRING:
        {
          std::cout << "in is string" << std::endl;
          std::string stringValue = messageValue["data"].asString();
          rpc->setData((char*)stringValue.c_str());
          break;
        }
        default:
          break;
      }

      std::string ret;
      usleep(500);

      switch(outtype)
      {
        case INT:
        {
          ret = std::to_string(rpc->getInt());
          break;
        }
        case FLOAT:
        {
          ret = std::to_string(rpc->getFloat());
          break;
        }
        case DOUBLE:
        {
          ret = std::to_string(rpc->getDouble());
          break;
        }
        case SHORT:
        {
          ret = std::to_string(rpc->getShort());
          break;
        }
        case LONGLONG:
        {
          ret = std::to_string(rpc->getLonglong());
          break;
        }
        case STRING:
        {
          std::cout << "out is string (2)" << std::endl;
          ret = rpc->getString();
          break;
        }
        default:
          break;
      }

      delete rpc;

      cout << "Server: Sending message \"" << ret <<  "\" to " << (size_t)connection.get() << endl;

      auto send_stream=make_shared<WsServer::SendStream>();
      *send_stream << ret;

      server.send(connection, send_stream, [](const boost::system::error_code& ec){
          if(ec) {
              cout << "Server: Error sending message. " <<
                      "Error: " << ec << ", error message: " << ec.message() << endl;
          }
      });
    };



    thread server_thread([&server](){
      //Start WS-server
      server.start();
    });

    //Wait for server to start so that the client can connect
    this_thread::sleep_for(chrono::seconds(1));


    server_thread.join();

    return 0;
}
