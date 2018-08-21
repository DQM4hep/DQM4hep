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
#include "dqm4hep/PluginManager.h"
#include "dqm4hep/OnlineElement.h"
#include "dqm4hep/OnlineRoutes.h"
#include "dqm4hep/XmlHelper.h"
#include "dqm4hep/DQM4hepConfig.h"

// -- std headers
#include <regex>

namespace dqm4hep {

  namespace online {
    
    ModuleApplication::ModuleApplication() :
      m_cycle(m_eventLoop) {
      m_monitorElementManager = std::make_shared<core::MonitorElementManager>();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    ModuleApplication::~ModuleApplication() {
      removeTimer(m_standaloneTimer);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::parseCmdLine(int argc, char **argv) {
      std::string cmdLineFooter = "Please report bug to <dqm4hep@desy.de>";
      m_cmdLine = std::make_shared<CmdLine::element_type>(cmdLineFooter, ' ', DQM4hep_VERSION_STR);

      TCLAP::ValueArg<std::string> steeringFileNameArg(
          "f"
          , "steering-file"
          , "The xml steering file for the module application"
          , true
          , ""
          , "string");
      m_cmdLine->add(steeringFileNameArg);

      core::StringVector verbosities(core::Logger::logLevels());
      TCLAP::ValuesConstraint<std::string> verbosityConstraint(verbosities);
      TCLAP::ValueArg<std::string> verbosityArg(
          "v"
          , "verbosity"
          , "The logging verbosity"
          , false
          , "info"
          , &verbosityConstraint);
      m_cmdLine->add(verbosityArg);
      
      TCLAP::ValueArg<std::string> moduleTypeArg(
          "t"
          , "type"
          , "The module type to run (plugin name). Overwrite the module type from steering file"
          , false
          , ""
          , "string");
      m_cmdLine->add(moduleTypeArg);
      
      TCLAP::ValueArg<std::string> moduleNameArg(
          "n"
          , "name"
          , "The module name to run. Overwrite the module name from steering file"
          , false
          , ""
          , "string");
      m_cmdLine->add(moduleNameArg);

      // TCLAP::MultiArg<std::string> parameterArg(
      //     "p"
      //     , "parameter"
      //     , "A parameter to replace in the application (see XmlHelper)"
      //     , false
      //     , "");
      // m_cmdLine->add(parameterArg);
      
      // parse command line
      m_cmdLine->parse(argc, argv);

      m_moduleType = moduleTypeArg.getValue();
      m_moduleName = moduleNameArg.getValue();
      
      setType(OnlineRoutes::ModuleApplication::applicationType());
      setLogLevel(core::Logger::logLevelFromString(verbosityArg.getValue()));

      parseSteeringFile(steeringFileNameArg.getValue());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::onInit() {
      dqm_info( "Module application running mode set to {0}", m_appRunningMode );
      m_standaloneTimer = createTimer();
      m_standaloneTimer->setInterval(m_standaloneSleep);
      m_standaloneTimer->setSingleShot(true);
      m_standaloneTimer->onTimeout().connect(this, &ModuleApplication::postStandaloneProcess);
      m_cycle.setEventPriority(Priorities::END_OF_CYCLE);
      m_runControl.onStartOfRun().connect(this, &ModuleApplication::setElementsRunNumber);
      m_runControl.onStartOfRun().connect(m_module.get(), &Module::startOfRun);  
      m_runControl.onEndOfRun().connect(m_module.get(), &Module::endOfRun);
      m_runControl.onStartOfRun().connect(this, &ModuleApplication::openArchive);
      m_runControl.onEndOfRun().connect(this, &ModuleApplication::archiveAndClose);
      if(ONLINE == appRunningMode()) {
        queuedSubscribe(
          OnlineRoutes::RunControl::sor(m_runControl.name()),
          Priorities::START_OF_RUN
        );
        queuedSubscribe(
          OnlineRoutes::RunControl::eor(m_runControl.name()),
          Priorities::END_OF_RUN
        );        
      }
      createQueuedCommand(
        OnlineRoutes::ModuleApplication::subscribe(name()),
        Priorities::SUBSCRIBE
      );
      if(EVENT_READER == appRunningMode()) {
        m_eventReader->onEventRead().connect(this, &ModuleApplication::receiveEvent);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::onEvent(AppEvent * appEvent) {
      if(AppEvent::SERVICE_UPDATE == appEvent->type()) {
        ServiceUpdateEvent* svc = dynamic_cast<ServiceUpdateEvent*>(appEvent);
        // Start of run
        if(svc->serviceName() == OnlineRoutes::RunControl::sor(m_runControl.name())) {
          processStartOfRun(svc);
        }
        // End of run
        if(svc->serviceName() == OnlineRoutes::RunControl::eor(m_runControl.name())) {
          processEndOfRun();
        }
      }
      if(AppEvent::COMMAND_HANDLING == appEvent->type()) {
        CommandEvent *cmd = dynamic_cast<CommandEvent*>(appEvent);
        // update subscription list
        if(cmd->commandName() == OnlineRoutes::ModuleApplication::subscribe(name())) {
          receiveSubscriptionList(cmd);
        }
      }
      if(AppEvent::END_OF_RUN == appEvent->type()) {
        auto eorEvent = dynamic_cast<StoreEvent<core::Run>*>(appEvent);
        auto run = eorEvent->data();
        m_runControl.endCurrentRun(run.parameters());
        if(EVENT_READER == appRunningMode()) {
          dqm_info( "End of run processed. Exiting application ..." );
          this->exit(0);
        }
      }
      // process event received from the event collector
      if(AppEvent::PROCESS_EVENT == appEvent->type() and ANALYSIS == appModuleType()) {  
        if(m_currentNQueuedEvents != 0) {
          m_currentNQueuedEvents--;
        }
        if(m_runControl.isRunning()) {
          auto procEvent = dynamic_cast<StoreEvent<core::EventPtr>*>(appEvent);
          auto anaModule = moduleAs<AnalysisModule>();
          anaModule->process(procEvent->data());
          m_cycle.incrementCounter();
          if(EVENT_READER == appRunningMode()) {
            auto status = m_eventReader->readNextEvent();
            // end of file ?
            if(status == core::STATUS_CODE_OUT_OF_RANGE) {
              processEndOfRun();
            }
            else if(status != core::STATUS_CODE_SUCCESS) {
              dqm_error( "Error while reading event: file reader returned status '{0}'", core::statusCodeToString(status) );
              this->exit(1);
            }
          }
        }
      }
      // generic loop
      if(AppEvent::PROCESS_EVENT == appEvent->type() and STANDALONE == appModuleType()) {
        auto standModule = moduleAs<StandaloneModule>();
        standModule->process();
        m_cycle.incrementCounter();
        m_standaloneTimer->start();
      }
      // end of cycle
      if(AppEvent::END_OF_CYCLE == appEvent->type()) {
        auto eocEvent = dynamic_cast<StoreEvent<EOCCondition>*>(appEvent);
        auto condition = eocEvent->data();
        m_module->endOfCycle(condition);
        if(condition.m_counter > 0) {
          try {
            core::QReportStorage reportStorage;
            // process quality tests
            THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_monitorElementManager->runQualityTests(reportStorage));
            core::json jreports;
            reportStorage.toJson(jreports);
            dqm_info( jreports.dump(2) );
            
            OnlineElementPtrList publishElements;
            m_monitorElementManager->iterate<OnlineElement>([&](OnlineElementPtr monitorElement){
              if(not monitorElement->publish() or not monitorElement->subscribed()){
                return true;                
              }
              publishElements.push_back(monitorElement);
              return true;
            });
            // TODO send monitor element to collector
          }
          catch(core::StatusCodeException &exception) {
            dqm_error( "Error caught at end of cycle: {0}", exception.getStatusCode() );
          }
        }
        // always restart a new cycle for standalone modules
        if(STANDALONE == appModuleType()) {
          m_module->startOfCycle();
          m_cycle.startCycle(true);
        }
        // forced end is a synonym of end of run for analysis modules
        // do not restart a cycle in this case
        if(ANALYSIS == appModuleType() and not condition.m_forcedEnd) {
          m_module->startOfCycle();
          m_cycle.startCycle(true); 
        }
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::onStart() {
      if(ONLINE == appRunningMode()) {
        // get run control status in case it is already running
        sendRequest(OnlineRoutes::RunControl::status(m_runControl.name()), net::Buffer(), [this](const net::Buffer &response){
          core::json statusJson = core::json::parse(response.begin(), response.end());
          bool rcRunning = statusJson.value<bool>("running", false);
          if(rcRunning) {
            core::json runJson = statusJson.value<core::json>("run", core::json({}));
            core::Run run;
            run.fromJson(runJson);
            m_runControl.startNewRun(run);
          }
        });        
      }
      if(STANDALONE == appModuleType()) {
        m_module->startOfCycle();
        m_cycle.startCycle(true);
        m_standaloneTimer->start();
      }
      else if(ANALYSIS == appModuleType() and m_runControl.isRunning()) {
        m_module->startOfCycle();
        m_cycle.startCycle(true);
      }
      if(EVENT_READER == appRunningMode()) {
        core::Run run;
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_eventReader->runInfo(run));
        m_runControl.startNewRun(run);
        m_module->startOfCycle();
        m_cycle.startCycle();
        // read first will post an event in event loop
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_eventReader->readNextEvent());
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::onStop() {
      if(STANDALONE == appModuleType()) {
        m_standaloneTimer->stop();
      }
    }
    
    //-------------------------------------------------------------------------------------------------
  
    ModulePtr ModuleApplication::module() const {
      return m_module;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &ModuleApplication::moduleName() const {
      return m_moduleName;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &ModuleApplication::moduleType() const {
      return m_moduleType;
    }

    //-------------------------------------------------------------------------------------------------

    ModuleApplication::RunningMode ModuleApplication::appRunningMode() const {
      return m_appRunningMode;
    }
    
    //-------------------------------------------------------------------------------------------------

    ModuleApplication::ModuleType ModuleApplication::appModuleType() const {
      return m_appModuleType;
    }

    //-------------------------------------------------------------------------------------------------

    const RunControl& ModuleApplication::runControl() const {
      return m_runControl;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool ModuleApplication::allowBooking() const {
      return m_allowBooking;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::shared_ptr<core::MonitorElementManager> ModuleApplication::monitorElementManager() const {
      return m_monitorElementManager;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::parseSteeringFile(const std::string &fname) {
      m_parser.parse(fname);
      
      auto document = m_parser.document();
      const core::TiXmlHandle xmlHandle(document.RootElement());
      auto storageElement = xmlHandle.FirstChildElement("storage").Element();
      auto settingsElement = xmlHandle.FirstChildElement("settings").Element();
      auto moduleElement = xmlHandle.FirstChildElement("module").Element();
      auto archiverElement = xmlHandle.FirstChildElement("archiver").Element();

      if(nullptr != storageElement) {
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_monitorElementManager->parseStorage<OnlineElement>(storageElement));
      }
                  
      if(nullptr == settingsElement) {
        dqm_error("parseSteeringFile: Missing <settings> section !");
        throw core::StatusCodeException(core::STATUS_CODE_NOT_FOUND);
      }
      
      // determine running mode
      static core::StringVector possibleModes = {"Online", "EventReader"}; 
      std::string runningMode;
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::getAttribute(settingsElement, 
        "mode", runningMode, [&](const std::string &value){
        return (std::find(possibleModes.begin(), possibleModes.end(), value) != possibleModes.end());
      }));
      if(runningMode == "Online") {
        m_appRunningMode = ONLINE;
      }
      else {
        m_appRunningMode = EVENT_READER;
      }
      
      configureModule(moduleElement);
      configureCycle(settingsElement);
      configureNetwork(settingsElement);
      configureEventReader(settingsElement);
      configureArchiver(archiverElement);
      
      core::TiXmlHandle settingsHandle(settingsElement);
      bool enableStatistics = false;
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND,!=, 
        core::XmlHelper::readParameter(settingsHandle, "StandaloneSleepTime", m_standaloneSleep));
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND,!=, 
        core::XmlHelper::readParameter(settingsHandle, "EnableStatistics", enableStatistics));
      enableStats(enableStatistics);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::configureModule(core::TiXmlElement *element) {
      if(m_moduleType.empty()) {
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::getAttribute(element, "type", m_moduleType));
      }
      if(m_moduleName.empty()) {
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::getAttribute(element, "name", m_moduleName));
      }
      if(m_moduleType.empty() or m_moduleName.empty()) {
        dqm_error( "Undefined module type and/or module name (type: {0}, name: {1}).", m_moduleType, m_moduleName );
        throw core::StatusCodeException(core::STATUS_CODE_FAILURE);
      }
      setName(m_moduleName);
      m_module = core::PluginManager::instance()->create<Module>(m_moduleType);
      if(nullptr == m_module) {
        dqm_error( "Module '{0}' not registered in plugin manager!", m_moduleType );
        dqm_error( "Please check your plugin settings (libraries, environment) and restart the application" );
        throw core::StatusCodeException(core::STATUS_CODE_NOT_FOUND);
      }
      if(nullptr != dynamic_cast<AnalysisModule*>(m_module.get())) {
        m_appModuleType = ANALYSIS;
      }
      else if(nullptr != dynamic_cast<StandaloneModule*>(m_module.get())) {
        m_appModuleType = STANDALONE;
      }
      else {
        dqm_error( "Undefined module type, must be AnalysisModule or StandaloneModule" );
        throw core::StatusCodeException(core::STATUS_CODE_INVALID_PARAMETER);
      }
      if(m_appRunningMode == EVENT_READER and m_appModuleType != ANALYSIS) {
        dqm_error( "Application running in FileReader mode. Can run only module of type AnalysisModule" );
        throw core::StatusCodeException(core::STATUS_CODE_INVALID_PARAMETER);
      }
      m_module->setModuleApplication(this);
      const core::TiXmlHandle moduleHandle(element);
      
      m_allowBooking = true;
      m_module->readSettings(moduleHandle);
      m_module->initModule();
      m_allowBooking = false;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::configureCycle(core::TiXmlElement *element) {
      unsigned int timeout = 10;
      unsigned int period = 30;
      unsigned int counterLimit = 0;
      if(nullptr != element) {
        core::TiXmlHandle handle(element);
        THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "CycleTimeout", timeout));
        THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "CyclePeriod", period));
        THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "CycleCounter", counterLimit));
      }
      if((timeout >= period) || (0 == period && 0 == counterLimit)) {
        dqm_error( "Invalid cycle settings: period={0}, timeout={1}, counter={2}", period, timeout, counterLimit );
        throw core::StatusCodeException(core::STATUS_CODE_INVALID_PARAMETER);
      }
      dqm_info( "== Cycle settings ==" );
      dqm_info( "=> Period:  {0}", period );
      dqm_info( "=> Timeout: {0}", timeout );
      dqm_info( "=> Counter: {0}", counterLimit );
      dqm_info( "====================" );
      m_cycle.setTimeout(timeout);
      m_cycle.setTimerPeriod(period);
      m_cycle.setCounterLimit(counterLimit);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::configureNetwork(core::TiXmlElement *element) {
      if(ONLINE != appRunningMode()) {
        return;
      }
      core::TiXmlHandle handle(element);
      std::string runControlName;
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::readParameter(handle, "RunControl", runControlName));
      m_runControl.setName(runControlName);
      
      if(ANALYSIS == appModuleType()) {
        std::string eventCollector;
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::readParameter(handle, "EventCollector", eventCollector));
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::readParameter(handle, "EventSource", m_eventSourceName));
        THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "EventQueueSize", m_eventQueueSize));
        
        m_eventCollectorClient = std::make_shared<EventClientPtr::element_type>(eventCollector);
        m_eventCollectorClient->onEventUpdate(m_eventSourceName, this, &ModuleApplication::receiveEvent);        
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::configureEventReader(core::TiXmlElement *element) {
      if(EVENT_READER != appRunningMode()) {
        return;
      }
      std::string eventReaderName, eventFileName;
      int skipNEvents = 0;
      core::TiXmlHandle handle(element);
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::readParameter(handle, "EventReader", eventReaderName));
      m_eventReader = core::PluginManager::instance()->create<core::EventReader>(eventReaderName);
      if(nullptr == m_eventReader) {
        dqm_error( "Could not found event reader {0} in list of plugins", eventReaderName );
        throw core::StatusCodeException(core::STATUS_CODE_NOT_FOUND);
      }
      dqm_info( "Loaded file reader plugin '{0}'", eventReaderName );
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, core::XmlHelper::readParameter(handle, "EventFileName", eventFileName));
      dqm_info( "Opening file: {0}", eventFileName );
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, core::XmlHelper::readParameter(handle, "SkipNEvents", skipNEvents));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_eventReader->open(eventFileName));
      if(0 != skipNEvents) {
        dqm_info( "Will skip {0} first events ...", skipNEvents );
        m_eventReader->skipNEvents(skipNEvents); // running offline ...
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::configureArchiver(core::TiXmlElement *element) {
      if(nullptr == element) {
        return;
      }
      bool enableArchiver = true;
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::getAttribute(element, "enable", enableArchiver));
      if(not enableArchiver) {
        dqm_info( "The archiver has been disabled. No monitor element will be archived !" );
        return;
      }
      core::TiXmlHandle handle(element);
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, 
        core::XmlHelper::readParameter(handle, "FileName", m_inputArchiveName));
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::readParameter(handle, "AllowOverwrite", m_allowOverwrite));
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::readParameter(handle, "AppendRunNumber", m_archiverRunNumber));
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::readParameter(handle, "WriteReferences", m_archiverWithReferences));
      THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
        core::XmlHelper::readParameter(handle, "OpenMode", m_archiveOpenMode));
      // finally create our archiver
      m_archiver = std::make_shared<core::Archiver>();
      auto selectorsElement = handle.FirstChildElement("selectors").Element();
      if(nullptr != selectorsElement) {
        for(auto selectorElement = selectorsElement->FirstChildElement("selector") ; 
          nullptr != selectorElement ; selectorElement = selectorElement->NextSiblingElement("selector")) {
          std::string expression;
          bool select = true;
          THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, 
            core::XmlHelper::getAttribute(selectorElement, "regex", expression));
          THROW_RESULT_IF_AND_IF(core::STATUS_CODE_SUCCESS, core::STATUS_CODE_NOT_FOUND, !=, 
            core::XmlHelper::getAttribute(selectorElement, "select", select));
          // try to compile the regex first
          try {
            std::regex reg(expression);
          }
          catch(std::regex_error) {
            dqm_error( "Archiver selector: invalid regular expression : {0}", expression );
            throw core::StatusCodeException(core::STATUS_CODE_INVALID_PARAMETER);
          }
          dqm_debug( "Archiver append selector, expression: {0}, select: {1}", expression, select );
          m_archiverSelector.addSelector([expression,select](core::MonitorElementPtr me)->bool{
            core::Path path = me->path();
            path += me->name();
            std::regex pattern(expression);
            const bool match = std::regex_match(path.getPath(), pattern);
            return select ? match : not match;
          });
        }
      }
      m_archiver->setSelectorFunction(m_archiverSelector.function());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::processStartOfRun(ServiceUpdateEvent *svc) {
      core::json runJson = core::json::parse(svc->buffer().begin(), svc->buffer().end());
      core::Run run;
      run.fromJson(runJson);
      dqm_info( "Starting new run {0}", run.runNumber() );
      m_runControl.startNewRun(run);
      if(ANALYSIS == appModuleType()) {
        m_module->startOfCycle();
        m_cycle.startCycle();
        if(ONLINE == appRunningMode()) {
          m_eventCollectorClient->startEventUpdates(m_eventSourceName);          
        }
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::processEndOfRun() {
      if(ANALYSIS == appModuleType() and ONLINE == appRunningMode()) {
        m_eventCollectorClient->stopEventUpdates(m_eventSourceName);
      }
      // end cycle only for analysis module case
      // standalone modules never stops
      if(ANALYSIS == appModuleType()) {
        m_cycle.forceStopCycle(true, true);
      }
      auto eorEvent = new StoreEvent<core::Run>(AppEvent::END_OF_RUN, m_runControl.currentRun());
      eorEvent->setPriority(ModuleApplication::END_OF_RUN);
      m_eventLoop.postEvent(eorEvent);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::receiveEvent(core::EventPtr event) {
      if(m_currentNQueuedEvents.load() >= m_eventQueueSize) {
        return;
      }
      auto appEvent = new StoreEvent<core::EventPtr>(AppEvent::PROCESS_EVENT, event);
      appEvent->setPriority(ModuleApplication::PROCESS_CALL);
      m_eventLoop.postEvent(appEvent);
      m_currentNQueuedEvents++;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::receiveSubscriptionList(CommandEvent *cmd) {
      core::json jsubscription = nullptr;
      try {
        jsubscription = core::json::parse(cmd->buffer().begin(), cmd->buffer().end());
      }
      catch(...) {
        dqm_error( "Caught exception: Couldn't update monitor element subscription list !" );
        return;
      }
      if(not jsubscription.is_array()) {
        dqm_error( "Monitor element subscription json object is not a list !" );
        return;
      }
      for(auto &element : jsubscription) {
        auto path = element.value<std::string>("path", "");
        auto eltName = element.value<std::string>("name", "");
        auto subscribe = element.value<bool>("sub", false);
        
        OnlineElementPtr monitorElement = nullptr;
        if(core::STATUS_CODE_SUCCESS != m_monitorElementManager->getMonitorElement(path, eltName, monitorElement)) {
          dqm_warning( "While updating subscription list: couldn't find element path'{0}', name '{1}'", path, eltName );
          continue;
        }
        monitorElement->setSubscribed(subscribe);
        dqm_info( "Monitor element: path {0}, name {1} has been {2}subscribed", path, eltName, subscribe ? "" : "un-" );
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::setElementsRunNumber(core::Run &run) {
      m_monitorElementManager->iterate<OnlineElement>([&](OnlineElementPtr monitorElement){
        monitorElement->setRunNumber(run.runNumber());
        return true;
      });
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::postStandaloneProcess() {
      auto *processEvent = new StoreEvent<core::time::point>(AppEvent::PROCESS_EVENT, core::time::now());
      processEvent->setPriority(ModuleApplication::PROCESS_CALL);
      m_eventLoop.postEvent(processEvent);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::openArchive(core::Run &run) {
      if(nullptr != m_archiver) {
        const int runNumber = m_archiverRunNumber ? run.runNumber() : -1;
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_archiver->open(m_inputArchiveName, m_archiveOpenMode, m_allowOverwrite, runNumber));        
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ModuleApplication::archiveAndClose(const core::Run &/*run*/) {
      if(nullptr != m_archiver) {
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_monitorElementManager->archive(*m_archiver, m_archiverWithReferences));
        THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_archiver->close());
      }
    }

  }

}

