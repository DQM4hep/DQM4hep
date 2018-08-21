//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_RUNCONTROLSERVER_H
#define DQM4HEP_RUNCONTROLSERVER_H

#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/RunControl.h>
#include <dqm4hep/Server.h>

#include <atomic>

namespace dqm4hep {

  namespace online {
    
    class RunControlInterface;

    /** 
     *  @brief  RunControlServer class
     *          The main run control server runner class.
     *          May be instantiated from a main().
     *          Act as a bridge between an external run control interface and
     *          the applications listenning to this run control. 
     */
    class RunControlServer {
    public:
      /** 
       *  @brief  Default constructor
       */
      RunControlServer() = default;
      RunControlServer(const RunControlServer&) = delete;
      RunControlServer& operator=(const RunControlServer&) = delete;

      /** 
       *  @brief  Destructor
       */
      ~RunControlServer() = default;

      /** 
       *  @brief  Set the run control name
       *          Must be called before run()
       *  
       *  @param  name the run control name
       */
      void setName(const std::string &name);

      /**
       *  @brief  Set the run control password
       *          Must be called before run()
       *
       *  @param  password the run control password 
       */
      void setPassword(const std::string &password);

      /** 
       *  @brief  Set the plugin name of the ouside interface
       *
       *  @param  name the run control interface plugin name to load from the
       *               plugin manager 
       */
      void setInterface(const std::string &name);
      
      /** 
       *  @brief  Set the user parameters
       *
       *  @param  parameters the user parameters, usually extracted from cmd line 
       */
      void setUserParameters(const dqm4hep::core::StringMap &parameters);

      /** 
       *  @brief  Run the server
       */
      void run();

      /** 
       *  @brief  Stop the server
       */
      void stop();
      
      /** 
       *  @brief  Get the run control
       */
      RunControl &runControl();

    private:
      void sor(dqm4hep::core::Run &run);
      void eor(const dqm4hep::core::Run &run);
      void sendCurrentRun(const dqm4hep::net::Buffer &request, dqm4hep::net::Buffer &response);

    private:
      typedef std::shared_ptr<RunControlInterface> RunControlInterfacePtr;
      typedef std::shared_ptr<dqm4hep::net::Server> ServerPtr;
      RunControl                    m_runControl = {};          ///< The main run control
      ServerPtr                     m_server = {nullptr};       ///< The server to run
      dqm4hep::net::Service        *m_pSorService = {nullptr};  ///< The "start of run" service 
      dqm4hep::net::Service        *m_pEorService = {nullptr};  ///< The "end of run" service
      RunControlInterfacePtr        m_interface = {nullptr};    ///< The external user plugin interface
      dqm4hep::core::StringMap      m_userParameters = {};      ///< The user parameters
      std::atomic<bool>             m_stopFlag = {false};       ///< The stop flag to run the server
      std::string                   m_interfaceName = {""};     ///< The external user interface plugin name 
    };

  }

}

#endif  //  DQM4HEP_RUNCONTROLSERVER_H
