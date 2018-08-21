//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dqm4hep/Client.h"

using namespace dqm4hep::net;

template <typename T>
void sendCommand(const Client &client, const std::string &name, const std::string &value);

template <typename T>
inline void sendCommand(const Client &client, const std::string &name, const std::string &value) {
  T tValue;
  std::stringstream ss;
  ss.str(value);

  if (!(ss >> tValue))
    throw;

  client.sendCommand(name, tValue, true);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cout << "Usage : dqm4hep-send-command command type data" << std::endl;
    return 1;
  }

  std::string name(argv[1]);
  std::string type(argv[2]);

  Client client;

  std::string str;

  for (int i = 3; i < argc; i++)
    str += argv[i] + std::string(" ");

  str.pop_back();

  if (type == "float")
    sendCommand<float>(client, name, str);
  else if (type == "int")
    sendCommand<int>(client, name, str);
  else if (type == "uint")
    sendCommand<unsigned int>(client, name, str);
  else if (type == "double")
    sendCommand<double>(client, name, str);
  else if (type == "short")
    sendCommand<short>(client, name, str);
  else if (type == "long")
    sendCommand<long>(client, name, str);
  else if (type == "ulong")
    sendCommand<unsigned long>(client, name, str);
  else if (type == "ullong")
    sendCommand<unsigned long long>(client, name, str);
  else if (type == "json")
    sendCommand<std::string>(client, name, str);
  else if (type == "string")
    sendCommand<std::string>(client, name, str);
  else
    throw "Command type error";

  return 0;
}
