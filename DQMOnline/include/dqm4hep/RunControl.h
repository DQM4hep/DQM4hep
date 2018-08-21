//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_RUNCONTROL_H
#define DQM4HEP_RUNCONTROL_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Signal.h>
#include <dqm4hep/Run.h>

namespace dqm4hep {

  namespace online {

    using StartOfRunSignal = dqm4hep::core::Signal<dqm4hep::core::Run &>;
    using EndOfRunSignal = dqm4hep::core::Signal<const dqm4hep::core::Run &>;

    /** 
     *  @brief  RunControl class.
     *          Implements the main logic of a run control.
     *          A run control has a name and optionally, a password to start/stop
     *          runs. One can use the SOR and EOR signals to be notified on start
     *          of run or end of run. E.g for SOR :
     *  @code
     *  void MyClass::onStartOfRun(Run &run) {
     *    dqm_info( "Run no {0} has been started !", run.runNumber() );
     *  }
     *
     *  void MyClass::onEndOfRun(const Run &run) {
     *    dqm_info( "Run no {0} ended !", run.runNumber() );
     *  }
     *  
     *  MyClass *usrObject = new MyClass();
     *  RunControl runControl;
     *  runControl.onStartOfRun().connect(usrObject, &MyClass::onStartOfRun);
     *  runControl.onEndOfRun().connect(usrObject, &MyClass::onEndOfRun);
     *  runControl.startNewRun(42); // onStartOfRun method called
     *  // ..
     *  runControl.endCurrentRun(); // onEndOfRun method called
     *  @endcode 
     */
    class RunControl {
    public:
      /** 
       *  @brief  Default constructor
       */
      RunControl() = default;

      /** 
       *  @brief  Constructor
       *
       *  @param  name the run control name
       */
      RunControl(const std::string &name);

      /** 
       *  @brief  Destructor
       */
      ~RunControl();

      /** 
       *  @brief  Set the run control name
       *
       *  @param  name the run control name
       */
      dqm4hep::core::StatusCode setName(const std::string &name);

      /** 
       *  @brief  Get the run control name
       */
      const std::string &name() const;

      /** 
       *  @brief  Start a new run
       *
       *  @param  run the run description
       *  @param  password the password to handle the run control, if any
       */
      dqm4hep::core::StatusCode startNewRun(const dqm4hep::core::Run &run, const std::string &password = "");

      /** 
       *  @brief  Start a new run.
       *          End the current run if already running
       *
       *  @param  runNumber the run number
       *  @param  description the run description
       *  @param  detectorName the run detector name
       *  @param  parameters additional run parameters
       *  @param  password the password to handle the run control, if any
       */
      dqm4hep::core::StatusCode startNewRun(int runNumber, const std::string &description = "", const std::string &detectorName = "", const dqm4hep::core::StringMap &parameters = dqm4hep::core::StringMap(), const std::string &password = "");

      /** 
       *  @brief  End the current run
       *
       *  @param  parameters optional parameters to end the run
       *  @param  password the password to handle the run control, if any
       */
      dqm4hep::core::StatusCode endCurrentRun(const dqm4hep::core::StringMap &parameters = dqm4hep::core::StringMap(), const std::string &password = "");

      /** 
       *  @brief  Get the current run
       */
      const dqm4hep::core::Run &currentRun() const;

      /** 
       *  @brief  Get the current run
       */
      dqm4hep::core::Run &currentRun();

      /** 
       *  @brief  Whether a run has been started.
       */
      bool isRunning() const;

      /** 
       *  @brief  Set the password needed to start/stop runs
       *          Throw exception if a run has already been started
       */
      void setPassword( const std::string &password );

      /** 
       *  @brief  Check the run control password
       */
      bool checkPassword(const std::string &password);

      /** 
       *  @brief  Get the start of run signal
       */
      StartOfRunSignal &onStartOfRun();

      /** 
       *  @brief  Get the end of run signal
       */
      EndOfRunSignal &onEndOfRun();

    protected:
      bool                             m_running = {false};                  ///< Whether a run has been started
      dqm4hep::core::Run               m_run = {dqm4hep::core::Run()};       ///< The main run object
      std::string                      m_name = {"Global"};                  ///< The run control name
      std::string                      m_password = {""};                    ///< An optional password to access the run control
      StartOfRunSignal                 m_sorSignal = {StartOfRunSignal()};   ///< The start of run signal
      EndOfRunSignal                   m_eorSignal = {EndOfRunSignal()};     ///< The end of run signal
    };

  }

}

#endif  //  DQM4HEP_RUNCONTROL_H
