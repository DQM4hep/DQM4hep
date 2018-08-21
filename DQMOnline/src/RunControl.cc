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
#include <dqm4hep/RunControl.h>
#include <dqm4hep/Run.h>
#include <dqm4hep/Logging.h>

// -- std headers
#include <algorithm>

using namespace dqm4hep::core;

namespace dqm4hep {

  namespace online {

    RunControl::RunControl(const std::string &rcname) :
        m_name(rcname) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    RunControl::~RunControl() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode RunControl::setName(const std::string &rcname) {
      if(rcname == m_name)
        return STATUS_CODE_SUCCESS;

      if(this->isRunning())
        return STATUS_CODE_NOT_ALLOWED;

      m_name = rcname;
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &RunControl::name() const {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode RunControl::startNewRun(const Run &run, const std::string &password) {
      if( ! this->checkPassword(password) )
        return STATUS_CODE_NOT_ALLOWED;

      if(this->isRunning())
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->endCurrentRun(dqm4hep::core::StringMap(), password));

      m_run = run;
      m_running = true;
      m_sorSignal.emit(m_run);
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode RunControl::startNewRun(int runNumber, const std::string &description, const std::string &detectorName, const dqm4hep::core::StringMap &parameters, const std::string &password) {
      if( ! this->checkPassword(password) )
        return STATUS_CODE_NOT_ALLOWED;

      if(this->isRunning())
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->endCurrentRun(dqm4hep::core::StringMap(), password));

      m_run = Run(runNumber, description, detectorName);
      
      for(auto &parameter : parameters)
        m_run.setParameter(parameter.first, parameter.second);
        
      m_sorSignal.emit(m_run);
      m_running = true;
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode RunControl::endCurrentRun(const dqm4hep::core::StringMap &parameters, const std::string &password) {
      if( ! this->checkPassword(password) )
        return STATUS_CODE_NOT_ALLOWED;

      if( ! this->isRunning() )
        return STATUS_CODE_SUCCESS;
        
      for(auto &parameter : parameters)
        m_run.setParameter(parameter.first, parameter.second);

      m_eorSignal.emit(m_run);
      m_running = false;
      m_run.reset();
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    const Run &RunControl::currentRun() const {
      return m_run;
    }

    //-------------------------------------------------------------------------------------------------

    Run &RunControl::currentRun() {
      return m_run;
    }

    //-------------------------------------------------------------------------------------------------

    bool RunControl::isRunning() const {
      return m_running;
    }

    //-------------------------------------------------------------------------------------------------

    void RunControl::setPassword(const std::string &password) {
      m_password = password;
    }

    //-------------------------------------------------------------------------------------------------

    bool RunControl::checkPassword(const std::string &password) {
      if( m_password.empty() ) {
        return true;
      }
      if( m_password == password ) {
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------------------------------

    StartOfRunSignal &RunControl::onStartOfRun() {
      return m_sorSignal;
    }

    //-------------------------------------------------------------------------------------------------

    EndOfRunSignal &RunControl::onEndOfRun() {
      return m_eorSignal;
    }

  }

}
