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

using namespace dqm4hep::net;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "usage : dqm4hep-server-running serverName" << std::endl;
    return -1;
  }

  std::cout << (Server::isServerRunning(argv[1]) ? 1 : 0) << std::endl;

  return 0;
}
