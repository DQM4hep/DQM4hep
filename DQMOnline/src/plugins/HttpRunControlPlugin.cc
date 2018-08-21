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
#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/RunControl.h"
#include "dqm4hep/RunControlInterface.h"
#include "dqm4hep/Run.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/PluginManager.h"

// -- std headers
#include <fstream>

// -- mongoose headers
#include <WebController.h>
#include <Server.h>

using namespace Mongoose;

namespace dqm4hep {

  namespace online {

    class HttpRunControl : public RunControlInterface, public Mongoose::WebController {
    public:      
      HttpRunControl() = default;
      ~HttpRunControl() = default;
      void readSettings(const dqm4hep::core::StringMap &parameters);
      bool runBlocking() const { return true; }
      void run();
      void stop();
      void setup();
      
    private:
      void receiveStartOfRunPost(Mongoose::Request &request, Mongoose::StreamResponse &response);
      void receiveEndOfRunPost(Mongoose::Request &request, Mongoose::StreamResponse &response);
      void getRunStatus(Mongoose::Request &request, Mongoose::StreamResponse &response);
      
      void printEor(const dqm4hep::core::Run &run);
      
    private:
      int                  m_port = {8000};
      atomic_bool          m_stopFlag = {false};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void HttpRunControl::readSettings(const dqm4hep::core::StringMap &parameters) {
      auto portIter = parameters.find("port");
      if(portIter != parameters.end()) {
        dqm4hep::core::stringToType(portIter->second, m_port);        
      }
      this->onEndOfRun().connect(this, &HttpRunControl::printEor);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void HttpRunControl::setup() {
      addRoute("POST", "/" + this->runControl().name() + "/SOR", HttpRunControl, receiveStartOfRunPost);
      addRoute("POST", "/" + this->runControl().name() + "/EOR", HttpRunControl, receiveEndOfRunPost);
      addRoute("GET",  "/" + this->runControl().name() + "/STATUS", HttpRunControl , getRunStatus );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void HttpRunControl::run() {
      Server srv(m_port);
      srv.registerController(this);
      srv.start();
      this->dumpRoutes();
      
      while(not m_stopFlag.load())
        usleep(1000);
      
      srv.stop();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void HttpRunControl::stop() {
      m_stopFlag = true;
    }

    //-------------------------------------------------------------------------------------------------

    void HttpRunControl::receiveStartOfRunPost(Mongoose::Request &request, Mongoose::StreamResponse &response) {
      dqm_info("Received POST: /{0}/SOR", this->runControl().name() );

      if(runControl().isRunning()) {
        response << "ERROR: Couldn't start new run, already running !" << std::endl;
        return;
      }

      std::map<std::string, std::string> variables(request.getAllVariable());
      std::map<std::string, std::string> urlVariables;
      request.getVars(urlVariables);

      dqm4hep::core::Run newRun;
      std::string password;
      bool consistent = false;

      for(auto iter = variables.begin(), endIter = variables.end() ; endIter != iter ; ++iter) {
        if(iter->first == "run") {
          int runNumber = 0;
          dqm4hep::core::stringToType(iter->second, runNumber);
          newRun.setRunNumber(runNumber);
          consistent = true;
          continue;
        }
        if(iter->first == "description") {
          newRun.setDescription(iter->second);
          continue;
        }
        if(iter->first == "detectorName") {
          newRun.setDetectorName(iter->second);
          continue;
        }
        if(iter->first == "password") {
          password = iter->second;
          continue;
        }
        newRun.setParameter(iter->first, iter->second);
      }
      if(not consistent) {
        response << "ERROR: Run number required to start a new run !!"<< std::endl;
        return;
      }
      try {
        this->startNewRun(newRun, password);
        dqm_info( "*** Started new run ***" );
        dqm_info( "{0}", core::typeToString(newRun) );
        dqm_info( "***********************" );
      }
      catch( dqm4hep::core::StatusCodeException &exception ) {
        response << "ERROR: Couldn't start new run ! What :" << exception.toString()<< std::endl;
        return;
      }
      response << "SUCCESS\n";
    }
    
    //-------------------------------------------------------------------------------------------------

    void HttpRunControl::receiveEndOfRunPost(Mongoose::Request &request, Mongoose::StreamResponse &response) {
      dqm_info("Received POST: /{0}/EOR", this->runControl().name());
      if(! this->runControl().isRunning()) {
        response << "ERROR: Not running ! Couldn't end current run !";
        return;
      }
      std::map<std::string, std::string> variables(request.getAllVariable());
      std::string password;
      dqm4hep::core::StringMap parameters;

      for(auto iter = variables.begin(), endIter = variables.end() ; endIter != iter ; ++iter) {
        if( iter->first == "password" ) {
          password = iter->second;
          continue;
        }
        parameters[iter->first] = iter->second;
      }

      try {
        this->endCurrentRun(parameters, password);
      }
      catch( dqm4hep::core::StatusCodeException &exception ) {
        response << "ERROR: Couldn't stop current run ! Exception: " << exception.toString() << std::endl;
        return;
      }

      response << "SUCCESS";
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void HttpRunControl::printEor(const dqm4hep::core::Run &r) {
      dqm_info( "****** Ended run ******" );
      dqm_info( "{0}", core::typeToString(r) );
      dqm_info( "***********************" );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void HttpRunControl::getRunStatus(Mongoose::Request &/*request*/, Mongoose::StreamResponse &response) {
      dqm_info("Received GET: /{0}/STATUS", this->runControl().name());
      
      core::json jsonRun;
      this->runControl().currentRun().toJson(jsonRun);
      core::json jsonInfo = {
        {"running", this->runControl().isRunning()},
        {"run", jsonRun}
      };
      response << jsonInfo.dump(2);
    }
    
    DQM_PLUGIN_DECL(HttpRunControl, "HttpRunControl");
  }

}
