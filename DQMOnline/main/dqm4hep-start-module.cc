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
#include "dqm4hep/ModuleApplication.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/PluginManager.h"

std::shared_ptr<dqm4hep::online::ModuleApplication> application;

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int) {
  dqm_info( "Caught CTRL+C. Stopping module application..." );
  if(application) {
    application->exit(0);
  }
}

//-------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  dqm4hep::core::screenSplash();
  
  // install signal handlers
  dqm_info( "Installing signal handlers ..." );
  signal(SIGINT,  int_key_signal_handler);
  
  // initialize and run the application
  int returnCode(0);
  application = std::make_shared<dqm4hep::online::ModuleApplication>();
  
  try {
    THROW_RESULT_IF(dqm4hep::core::STATUS_CODE_SUCCESS, !=, dqm4hep::core::PluginManager::instance()->loadLibraries());
    application->init(argc, argv);
  }
  catch(dqm4hep::core::StatusCodeException &e) {
    dqm_error( "init: Caught StatusCodeException: '{0}'", e.toString() );
    return e.getStatusCode();
  }
  catch(...) {
    dqm_error( "init: Caught unknown exception" );
    return 1;
  }
  
  try {
    returnCode = application->exec();
  }
  catch(dqm4hep::core::StatusCodeException &e) {
    dqm_error( "exec: Caught StatusCodeException: '{0}'", e.toString() );
    return e.getStatusCode();
  }
  catch(...) {
    dqm_error( "exec: Caught unknown exception" );
    return 1;
  }
  
  return returnCode;
}
