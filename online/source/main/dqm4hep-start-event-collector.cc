/// \file dqm4hep-start-event-collector.cc
/*
 *
 * dqm4hep-start-event-collector.cc main source file template automatically generated
 * Creation date : mer. nov. 5 2014
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

// -- dqm4hep headers
#include "dqm4hep/EventCollector.h"
#include "dqm4hep/Logging.h"

std::shared_ptr<dqm4hep::online::EventCollector> application;

//-------------------------------------------------------------------------------------------------

// key interrupt signal handling
void int_key_signal_handler(int) {
  dqm_info( "Caught CTRL+C. Stopping event collector..." );
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
  application = std::make_shared<dqm4hep::online::EventCollector>();
  
  try {
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
