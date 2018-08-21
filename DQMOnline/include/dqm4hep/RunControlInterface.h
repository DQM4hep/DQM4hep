//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_RUNCONTROLINTERFACE_H
#define DQM4HEP_RUNCONTROLINTERFACE_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Run.h>
#include <dqm4hep/RunControl.h>

namespace dqm4hep {

  namespace online {
    
    class RunControlServer;

    /** 
     *  @brief  RunControlInterface class.
     *          Main interface to external user run control.
     *          User must implement this interface and declare a plugin instance
     *          using the DQM_PLUGIN_DECL macro. 
     *          An instance of this class is allocated by a RunControlServer by 
     *          looking for the plugin name in the plugin registry.    
     */
    class RunControlInterface
    {
      friend class RunControlServer;
    public:
      /** 
       *  @brief  Destructor
       */
      virtual ~RunControlInterface() {}
      
      /** 
       *  @brief  Read settings from map
       *
       *  @param parameters a key/value map with user settings (string to string)
       */
      virtual void readSettings(const dqm4hep::core::StringMap &parameters) = 0;
      
      /** 
       *  @brief  Whether calling the run() method is blocking.
       *          This mainly depends on whether the external interface is
       *          implemented in a separated thread. In this case, return false.
       */
      virtual bool runBlocking() const = 0;
      
      /** 
       *  @brief  Run the user run control
       */
      virtual void run() = 0;

      /** 
       *  @brief  Stop the user run control
       */
      virtual void stop() = 0;

    protected:
      /** 
       *  @brief  Start a new run (dqm4hep::online::RunControl class).
       *          A passwork may be specified if the run control application has
       *          been started with a password
       *
       *  @param  run the run description
       *  @param  password the password to handle the run control, if any
       */
      void startNewRun(const dqm4hep::core::Run &run, const std::string &password = "");
      
      /** 
       *  @brief  End the current run
       *          A passwork may be specified if the run control application has
       *          been started with a password
       *
       *  @param  parameters optional parameters to end the run
       *  @param  password the password to handle the run control, if any
       */
      void endCurrentRun(const dqm4hep::core::StringMap &parameters = dqm4hep::core::StringMap(), const std::string &password = "");
      
      /** 
       *  @brief  Get the run control
       */
      const RunControl& runControl() const;
      
      /** 
       *  @brief  Get the run control start of run signal
       */
      StartOfRunSignal &onStartOfRun();
      
      /** 
       *  @brief  Get the run control end of run signal
       */
      EndOfRunSignal &onEndOfRun();
      
    private:
      /** 
       *  @brief  Set the run control server
       *
       *  @param  srv the run control server owning the interface 
       */
      void setServer(RunControlServer *srv);
      
    private:
      RunControlServer     *m_pServer = nullptr;      ///< The run control server owning the interface
    };

  }

}

#endif  //  DQM4HEP_RUNCONTROLINTERFACE_H
