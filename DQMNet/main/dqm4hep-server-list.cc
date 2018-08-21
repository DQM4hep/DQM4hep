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

int main(int /*argc*/, char ** /*argv*/) {
  std::vector<std::string> runningServers(Server::runningServers());

  for (auto iter = runningServers.begin(), endIter = runningServers.end(); endIter != iter; ++iter)
    std::cout << *iter << " ";

  std::cout << std::endl;

  return 0;
}
