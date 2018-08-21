//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_EVENTCOLLECTOR_H
#define DQM4HEP_EVENTCOLLECTOR_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/Application.h"

// -- tclap headers
#include "tclap/CmdLine.h"
#include "tclap/Arg.h"

namespace dqm4hep {

  namespace online {
    
    /** 
     *  @brief  EventCollector class
     */
    class EventCollector : public Application
    {
    public:
      /**
       *  @brief  Default constructor
       */
      EventCollector();
      EventCollector(const EventCollector&) = delete;
      EventCollector& operator=(const EventCollector&) = delete;
      
      /**
       *  @brief  Default destructor
       */
      ~EventCollector();
      
      void parseCmdLine(int argc, char **argv) override;
      void onInit() override;
      void onEvent(AppEvent *pAppEvent) override;
      void onStart() override;
      void onStop() override;

    private:
      void handleRegistration(const net::Buffer &request, net::Buffer &response);
      void handleClientExit(StoreEvent<int> *event);
      void handleCollectEvent(const net::Buffer &buffer);
      void handleClientUnregistration(const net::Buffer &buffer);
      void handleEventRequest(const net::Buffer &request, net::Buffer &response);
      void sendStatsTimer10();
      void sendStatsTimer60();
      void printSourceMap();
      
      /**
       *  @brief  SourceInfo struct
       */
      struct SourceInfo {
        SourceInfo() = default;
        SourceInfo(SourceInfo&& info);
        SourceInfo(const SourceInfo&) = delete;
        SourceInfo& operator=(const SourceInfo&) = delete;
        
        int                  m_clientId = {0};
        std::string          m_name = {""};
        std::string          m_streamerName = {""};
        core::StringVector   m_collectors = {};
        core::StringMap      m_hostInfo = {};
        net::Buffer          m_buffer = {};
        net::Service        *m_eventService = {nullptr};
      };
      
      typedef std::map<std::string, SourceInfo> SourceInfoMap;
      
      std::shared_ptr<TCLAP::CmdLine>     m_cmdLine = nullptr;
      SourceInfoMap                       m_sourceInfoMap = {};
      core::time::point                   m_lastStatCall10 = {};
      core::time::point                   m_lastStatCall60 = {};
      unsigned int                        m_nCollectedEvents10 = {0};
      unsigned int                        m_nCollectedEvents60 = {0};
      unsigned int                        m_nCollectedBytes10 = {0};
      unsigned int                        m_nCollectedBytes60 = {0};
      AppTimer*                           m_statsTimer10 = {nullptr};
      AppTimer*                           m_statsTimer60 = {nullptr};
    };

  }

} 

#endif  //  DQM4HEP_EVENTCOLLECTOR_H
