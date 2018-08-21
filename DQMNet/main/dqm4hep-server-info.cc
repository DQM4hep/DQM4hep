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
using namespace dqm4hep::core;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage : dqm4hep-server-info servername" << std::endl;
    return 1;
  }

  std::string serverName(argv[1]);

  Client client;
  json serverInfo;
  client.queryServerInfo(serverName, serverInfo);
  std::cout << serverInfo.dump(2) << std::endl;

  return 0;
}
