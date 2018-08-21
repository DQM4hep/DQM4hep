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
#include "dqm4hep/EventCollector.h"
#include "dqm4hep/DQM4hepConfig.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/OnlineRoutes.h"

namespace dqm4hep {

  namespace online {
    
    EventCollector::EventCollector() : 
      Application() {
    }
    
    //-------------------------------------------------------------------------------------------------
    
    EventCollector::~EventCollector() {
      removeTimer(m_statsTimer10);
      removeTimer(m_statsTimer60);
    }

    //-------------------------------------------------------------------------------------------------

    void EventCollector::parseCmdLine(int argc, char **argv) {
      std::string cmdLineFooter = "Please report bug to <dqm4hep@gmail.com>";
      m_cmdLine = std::make_shared<TCLAP::CmdLine>(cmdLineFooter, ' ', DQM4hep_VERSION_STR);
      
      TCLAP::ValueArg<std::string> collectorNameArg(
          "c"
          , "collector-name"
          , "The event collector name"
          , true
          , ""
          , "string");
      m_cmdLine->add(collectorNameArg);
      
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
      
      // parse command line
      m_cmdLine->parse(argc, argv);

      std::string verbosity(verbosityArg.getValue());
      std::string collectorName(collectorNameArg.getValue());
      setType(OnlineRoutes::EventCollector::applicationType());
      setName(collectorName);
      setLogLevel(core::Logger::logLevelFromString(verbosity));
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::onInit() {
      // create network services
      createRequestHandler(
        OnlineRoutes::EventCollector::registerSource(name()), 
        this, 
        &EventCollector::handleRegistration
      );
      createRequestHandler(
        OnlineRoutes::EventCollector::eventRequest(name()), 
        this, 
        &EventCollector::handleEventRequest
      );
      createDirectCommand(
        OnlineRoutes::EventCollector::unregisterSource(name()), 
        this, 
        &EventCollector::handleClientUnregistration
      );
      createDirectCommand(
        OnlineRoutes::EventCollector::collectEvent(name()), 
        this, 
        &EventCollector::handleCollectEvent
      );
      
      // create statistics entries
      createStatsEntry("NSources", "", "The current number of registered sources");
      createStatsEntry("NEvents_60sec", "1/min", "The number of collected events within the last minute");
      createStatsEntry("NEvents_10sec", "1/10 sec", "The number of collected events within the last 10 secondes");
      createStatsEntry("NBytes_60sec", "bytes", "The total number of collected bytes within the last minute");
      createStatsEntry("NBytes_10sec", "bytes", "The total number of collected bytes within the last 10 secondes");
      createStatsEntry("NMeanBytes_60sec", "bytes/min", "The mean number of collected bytes within the last minute");
      createStatsEntry("NMeanBytes_10sec", "bytes/10 sec", "The mean number of collected bytes within the last 10 secondes");
      
      // app stats timers
      m_statsTimer10 = createTimer();
      m_statsTimer10->setInterval(10000);
      m_statsTimer10->setSingleShot(false);
      m_statsTimer10->onTimeout().connect(this, &EventCollector::sendStatsTimer10);
      m_statsTimer60 = createTimer();
      m_statsTimer60->setInterval(60000);
      m_statsTimer60->setSingleShot(false);
      m_statsTimer60->onTimeout().connect(this, &EventCollector::sendStatsTimer60);
      
      m_lastStatCall10 = core::now();
      m_lastStatCall60 = core::now();
      
      m_statsTimer10->start();
      m_statsTimer60->start();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::onEvent(AppEvent *pAppEvent) {
      if(pAppEvent->type() == AppEvent::CLIENT_EXIT) {
        auto exitEvent = dynamic_cast<StoreEvent<int>*>(pAppEvent);
        this->handleClientExit(exitEvent);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::onStart() {
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::onStop() {
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::handleRegistration(const net::Buffer &request, net::Buffer &response) {
      core::json registrationDetails({});
      if(0 != request.size()) {
        registrationDetails = core::json::parse(request.begin(), request.end());   
      }
      auto clientSourceName = registrationDetails.value<std::string>("source", "");
      auto clientId = this->serverClientId();  
      auto findIter = m_sourceInfoMap.find(clientSourceName);
      core::json clientResponseValue({});
      
      // source already registered
      if(m_sourceInfoMap.end() != findIter) {
        if(clientId == findIter->second.m_clientId) {
          clientResponseValue["message"] = "Event source already registered !";
          clientResponseValue["registered"] = true;
        }
        else {
          std::stringstream ss; ss << "Event source already registered with a different client ID (" << findIter->second.m_clientId << ") !";
          clientResponseValue["message"] = ss.str();
          clientResponseValue["registered"] = false;
        }
      }
      // source not registered yet
      else {
        std::string sourceName = registrationDetails.value<std::string>("source", "");
        SourceInfo sourceInfo;
        findIter = m_sourceInfoMap.insert(SourceInfoMap::value_type(sourceName, std::move(sourceInfo))).first;
        
        findIter->second.m_clientId = clientId;
        findIter->second.m_name = registrationDetails.value<std::string>("source", "");
        findIter->second.m_streamerName = registrationDetails.value<std::string>("streamer", "");
        findIter->second.m_eventService = createService(OnlineRoutes::EventCollector::eventUpdate(name(), findIter->first));
        
        auto collectors = registrationDetails["collectors"];
        auto hostInfo = registrationDetails["host"];
        
        for(auto collector : collectors) {
          findIter->second.m_collectors.push_back(collector.get<std::string>());
        }
          
        for(auto hostInfoIter : hostInfo.items()) {
          findIter->second.m_hostInfo[hostInfoIter.key()] = hostInfo[hostInfoIter.key()].get<std::string>();
        }
        
        auto model = findIter->second.m_buffer.createModel<std::string>();
        findIter->second.m_buffer.setModel(model);
        
        dqm_info( "New event source '{0}' registered with client id {1}", findIter->second.m_name, findIter->second.m_clientId );
        
        clientResponseValue["registered"] = true;
        sendStat("NSources", m_sourceInfoMap.size());
      }
      
      auto model = response.createModel<std::string>();
      model->copy(clientResponseValue.dump());
      response.setModel(model);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::handleClientExit(StoreEvent<int> *event) {
      const int clientId(event->data());
      auto findIter = std::find_if(m_sourceInfoMap.begin(), m_sourceInfoMap.end(), [&clientId](const SourceInfoMap::value_type &iter){
        return (iter.second.m_clientId == clientId);
      });
      
      if(findIter != m_sourceInfoMap.end()) {
        dqm_info( "Removing event source '{0}' from source list !", findIter->second.m_name );
        m_sourceInfoMap.erase(findIter);
        sendStat("NSources", m_sourceInfoMap.size());
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::handleCollectEvent(const net::Buffer &buffer) {
      const int clientId(serverClientId());
      
      auto findIter = std::find_if(m_sourceInfoMap.begin(), m_sourceInfoMap.end(), [&clientId](const SourceInfoMap::value_type &iter){
        return (iter.second.m_clientId == clientId);
      });
      
      if(findIter != m_sourceInfoMap.end()) {          
        std::string copiedBuffer(buffer.begin(), buffer.size());
        auto newModel = findIter->second.m_buffer.createModel<std::string>();
        findIter->second.m_buffer.setModel(newModel);
        newModel->move(std::move(copiedBuffer));
        
        m_nCollectedEvents10++;
        m_nCollectedEvents60++;
        m_nCollectedBytes10 += buffer.size();
        m_nCollectedBytes60 += buffer.size();
        // send update
        findIter->second.m_eventService->sendBuffer(buffer.begin(), buffer.size());
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::handleClientUnregistration(const net::Buffer &/*buffer*/) {
      const int clientId(serverClientId());
      auto findIter = std::find_if(m_sourceInfoMap.begin(), m_sourceInfoMap.end(), [&clientId](const SourceInfoMap::value_type &iter){
        return (iter.second.m_clientId == clientId);
      });
      
      if(findIter != m_sourceInfoMap.end()) {
        dqm_info( "Removing event source '{0}' from source list !", findIter->second.m_name );
        m_sourceInfoMap.erase(findIter);
        sendStat("NSources", m_sourceInfoMap.size());
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::handleEventRequest(const net::Buffer &request, net::Buffer &response) {
      std::string sourceName(request.begin(), request.size());
      auto findIter = m_sourceInfoMap.find(sourceName);
      if(m_sourceInfoMap.end() != findIter) {
        response.setModel(findIter->second.m_buffer.model());
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::sendStatsTimer10() {
      auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(core::now()-m_lastStatCall10).count();
      // send stats
      sendStat("NEvents_10sec", m_nCollectedEvents10);
      sendStat("NBytes_10sec", m_nCollectedBytes10);
      sendStat("NMeanBytes_10sec", m_nCollectedBytes10 / (timeDifference/1000.));
      // reset counters
      m_nCollectedEvents10 = 0;
      m_nCollectedBytes10 = 0;
      m_lastStatCall10 = core::now();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::sendStatsTimer60() {
      auto timeDifference = std::chrono::duration_cast<std::chrono::milliseconds>(core::now()-m_lastStatCall60).count();
      // send stats
      sendStat("NEvents_60sec", m_nCollectedEvents60);
      sendStat("NBytes_60sec", m_nCollectedBytes60);
      sendStat("NMeanBytes_60sec", m_nCollectedBytes60 / (timeDifference/1000.));
      // reset counters
      m_nCollectedEvents60 = 0;
      m_nCollectedBytes60 = 0;
      m_lastStatCall60 = core::now();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollector::printSourceMap() {
      for(auto &source : m_sourceInfoMap) {
        dqm_debug( "== Source '{0}' ==", source.first );
        dqm_debug( "     Client id: '{0}' ==", source.second.m_clientId );
        dqm_debug( "     Streamer:  '{0}' ==", source.second.m_streamerName );
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    EventCollector::SourceInfo::SourceInfo(EventCollector::SourceInfo&& info) :
      m_clientId(std::move(info.m_clientId)),
      m_name(std::move(info.m_name)),
      m_streamerName(std::move(info.m_streamerName)),
      m_collectors(std::move(info.m_collectors)),
      m_hostInfo(std::move(info.m_hostInfo)),
      m_buffer(std::move(info.m_buffer)),
      m_eventService(info.m_eventService) {
      info.m_eventService = nullptr;
    }

  }

}
