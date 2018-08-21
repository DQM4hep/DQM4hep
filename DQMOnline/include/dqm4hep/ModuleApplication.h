//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_MODULEAPPLICATION_H
#define DQM4HEP_MODULEAPPLICATION_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/RunControl.h"
#include "dqm4hep/XMLParser.h"
#include "dqm4hep/Application.h"
#include "dqm4hep/Cycle.h"
#include "dqm4hep/Module.h"
#include "dqm4hep/EventCollectorClient.h"
#include "dqm4hep/MonitorElementManager.h"
#include "dqm4hep/EventReader.h"
#include "dqm4hep/Archiver.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

namespace dqm4hep {

  namespace online {
    
    class ModuleApi;

    /** 
     *  @brief  ModuleApplication class
     */
    class ModuleApplication : public Application {
      friend class ModuleApi;
    public:
      /**
       *  @brief  Mode enumerator
       */
      enum ModuleType {
        UNDEFINED_TYPE,
        ANALYSIS,
        STANDALONE
      };
      
      enum RunningMode {
        UNDEFINED_MODE,
        ONLINE,
        EVENT_READER
      };
      
      ModuleApplication(const ModuleApplication&) = delete;
      ModuleApplication& operator=(const ModuleApplication&) = delete;
      
      /** 
       *  @brief  Constructor
       */
      ModuleApplication();

      /** 
       *  @brief  Default destructor
       */
      ~ModuleApplication();

      /** 
       *  @brief  Get the module managed by this application
       */
      ModulePtr module() const;
      
      /**
       *  @brief  Get a pointer to the user module dynamic casted to a certain type
       */
      template <typename T>
      T *moduleAs();

      /** 
       *  @brief  Get the module name
       */
      const std::string &moduleName() const;

      /** 
       *  @brief  Get the module type
       */
      const std::string &moduleType() const;
      
      /**
       *  @brief  Get the application running mode
       */
      RunningMode appRunningMode() const;
      
      /**
       *  @brief  Get the application module type
       */
      ModuleType appModuleType() const;
      
      /**
       *  @brief  Get the run control
       */
      const RunControl& runControl() const;
      
      /**
       *  @brief  Whether the application the application is in a state where booking monitor elements is allowed
       */
      bool allowBooking() const;
      
    private:
      void parseCmdLine(int argc, char **argv) override;
      void onInit() override;
      void onEvent(AppEvent *pAppEvent) override;
      void onStart() override;
      void onStop() override;
      
      /**
       *  @brief  Parse the input steering file
       *
       *  @param  fname the file name to parse
       */
      void parseSteeringFile(const std::string &fname);
      
      /**
       *  @brief  Configure the user module
       *
       *  @param  element the xml element
       */
      void configureModule(core::TiXmlElement *element);
      
      /**
       *  @brief  Configure the application cycle
       *
       *  @param  element the xml element
       */
      void configureCycle(core::TiXmlElement *element);
      
      /**
       *  @brief  Configure the network interface
       *
       *  @param  element the xml element
       */
      void configureNetwork(core::TiXmlElement *element);
      
      /**
       *  @brief  Configure the event reader
       *
       *  @param  element the xml element
       */
      void configureEventReader(core::TiXmlElement *element);
      
      /**
       *  @brief  Configure the archiver
       *
       *  @param  element the xml element
       */
      void configureArchiver(core::TiXmlElement *element);
      
      /**
       *  @brief  Process the start of run service update
       *
       *  @param  svc the service update content
       */      
      void processStartOfRun(ServiceUpdateEvent *svc);
      
      /**
       *  @brief  Process the end of run service update
       */
      void processEndOfRun();
      
      /**
       *  @brief  Receive an event from event collector and post it in the event loop
       *
       *  @param  event an event from the event collector
       */
      void receiveEvent(core::EventPtr event);
      
      /**
       *  @brief  Receive the new monitor element subscription list
       *  
       *  @param  cmd the network command buffer (json) 
       */
      void receiveSubscriptionList(CommandEvent *cmd);
      
      /**
       *  @brief  Slot to set the run number of all monitor elements on start of run
       *  
       *  @param  run the run description
       */
      void setElementsRunNumber(core::Run &run);
      
      /**
       *  @brief  Get the monitor element manager
       */
      std::shared_ptr<core::MonitorElementManager> monitorElementManager() const;
      
      /**
       *  @brief  Slot to post an event on standalone timer timeout
       */
      void postStandaloneProcess();
      
      /**
       *  @brief  Open a new archive
       * 
       *  @param  run the run description on start of run
       */
      void openArchive(core::Run &run);
      
      /**
       *  @brief  Archive the current storage and close the archivve
       *  
       *  @param  run the run description on end of run
       */
      void archiveAndClose(const core::Run &run);
    
    private:  
      using CmdLine = std::shared_ptr<TCLAP::CmdLine>;
      using EventClientPtr = std::shared_ptr<EventCollectorClient>;
      using MonitorElementManagerPtr = std::shared_ptr<core::MonitorElementManager>;
      using EventReaderPtr = std::shared_ptr<core::EventReader>;
      using ArchiverPtr = std::shared_ptr<core::Archiver>;
      
      /**
       *  @brief  Priorities enumerator
       */
      enum Priorities {
        PROCESS_CALL = 50,
        END_OF_RUN = 60,
        END_OF_CYCLE = 70,
        SUBSCRIBE = 75,
        START_OF_RUN = 80
      };

    private:
      /// The user module type (plugin name)
      std::string                  m_moduleType = {""};
      /// The user module name (runtime defined)
      std::string                  m_moduleName = {""};
      /// The application run control
      RunControl                   m_runControl = {};
      /// The application running mode
      RunningMode                  m_appRunningMode = {UNDEFINED_MODE};
      /// The application module type mode
      ModuleType                   m_appModuleType = {UNDEFINED_TYPE};
      /// The command line argument object
      CmdLine                      m_cmdLine = {nullptr};
      /// The XML parser to parse the input steering file
      core::XMLParser              m_parser = {};
      /// The user module plugin 
      ModulePtr                    m_module = {nullptr};
      /// The cycle object managing cycles in the application
      Cycle                        m_cycle;
      /// The event collector client to receive events from collector
      EventClientPtr               m_eventCollectorClient = {nullptr};
      /// The event source from the event collector
      std::string                  m_eventSourceName = {""};
      /// The current number of events in the event loop
      std::atomic_uint             m_currentNQueuedEvents = {0};
      /// The maximum of queued events to be processed (sub-sampling)
      unsigned int                 m_eventQueueSize = {100};
      /// The timer for the standalone module mode
      AppTimer*                    m_standaloneTimer = {nullptr};
      /// The time between two consecutive standalone module process (unit ms)
      unsigned int                 m_standaloneSleep = {1000};
      /// The monitor element manager
      MonitorElementManagerPtr     m_monitorElementManager = {nullptr};
      /// Whether the application for monitor element booking (state variable)
      bool                         m_allowBooking = {false};
      /// The event reader 
      EventReaderPtr               m_eventReader = {nullptr};
      /// The archiver file name as read from config
      std::string                  m_inputArchiveName = {""};
      /// The archiver open mode
      std::string                  m_archiveOpenMode = {"RECREATE"};
      /// Whether to name the archive file with run number
      bool                         m_archiverRunNumber = {true};
      /// Whether to archive references
      bool                         m_archiverWithReferences = {false};
      /// Whether to allow for archive overwrite
      bool                         m_allowOverwrite = {false};
      /// The archiver selector
      core::ArchiverSelector       m_archiverSelector = {};
      /// The monitor element archiver
      ArchiverPtr                  m_archiver = {nullptr};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline T *ModuleApplication::moduleAs() {
      return dynamic_cast<T*>(m_module.get());
    }

  }

} 

#endif  //  DQM4HEP_MODULEAPPLICATION_H
