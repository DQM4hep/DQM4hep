//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/StatusCodes.h>

using namespace std;
using namespace dqm4hep::core;

int main(int /*argc*/, char ** /*argv*/) {
  
  try {
    THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PluginManager::instance()->loadLibraries());
    PluginManager::instance()->dump();
    PluginManager::kill();
  } 
  catch (StatusCodeException &e) {
    DQM4HEP_NO_EXCEPTION( dqm_error("While loading libraries : Caught {0}", e.toString()); );
    PluginManager::kill();
    return e.getStatusCode();
  } 
  catch (...) {
    dqm_error("While loading libraries : Caught unknown error");
    PluginManager::kill();
    return 1;
  }
  
  return 0;
}
