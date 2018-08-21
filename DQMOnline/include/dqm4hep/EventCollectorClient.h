//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_EVENTCOLLECTORCLIENT_H
#define DQM4HEP_EVENTCOLLECTORCLIENT_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/EventStreamer.h"
#include "dqm4hep/Client.h"

// -- root headers
#include <TBufferFile.h>

// -- std headers
#include <mutex>

namespace dqm4hep {

  namespace online {
    
    /** 
     *  @brief  EventCollectorClient class
     */
    class EventCollectorClient {
    public:
      ~EventCollectorClient() = default;
      
      /**
       *  @brief  Default constructor
       *
       *  @param  collectorName the collector name to connect to 
       */
      EventCollectorClient(const std::string &collectorName);
      
      /**
       *  @brief  Request the last collected event in the collector
       */
      core::EventPtr queryEvent(const std::string &source);
      
      /**
       *  @brief  Get the signal for event updates for a given source
       *          If the source is not yet registered internally here,
       *          it is created and returned in any case
       *
       *  @param  source the source name
       *  @param  controller the object receiving the event
       *  @param  function the object function receiving the event
       */
      template <typename Controller>
      void onEventUpdate(const std::string &source, Controller *controller, void (Controller::*function)(core::EventPtr event));
      
      /**
       *  @brief  Instruct the event collector to send the latest received 
       *          event when a new one is available for a given source
       *
       *  @param  source the source name
       */
      void startEventUpdates(const std::string &source);
      
      /**
       *  @brief  Instruct the event collector to stop sending event
       */
      void stopEventUpdates(const std::string &source);
      
      /**
       *  @brief  Whether the client is currently receiving event updates from the collector
       */
      bool receivingEventUpdates(const std::string &source) const;
      
      void startEventUpdates();
      void stopEventUpdates();
      
    private:
      void setUpdateMode(const std::string &source, bool receiveUpdates);
      core::EventPtr readEvent(const net::Buffer &buffer);

    private:
      using EventUpdateSignal = core::Signal<core::EventPtr>;
      
      /**
       *  @brief  SourceInfo struct
       */
      struct SourceInfo {
        EventUpdateSignal       m_eventUpdateSignal = {};      ///< The signal to process on event update
        EventCollectorClient   *m_collectorClient = {nullptr};
        void receiveEvent(const net::Buffer &buffer);
      };
      friend struct SourceInfo;
      using SourceInfoMap = std::map<std::string, SourceInfo>;
      
    private:
      std::string                         m_collectorName = {""};
      SourceInfoMap                       m_sourceInfoMap = {};
      net::Client                         m_client = {};
      mutable std::recursive_mutex        m_mutex = {};
      core::EventStreamer                 m_eventStreamer = {};
      TBufferFile                         m_buffer = {TBuffer::kRead};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename Controller>
    inline void EventCollectorClient::onEventUpdate(const std::string &source, Controller *controller, void (Controller::*function)(core::EventPtr event)) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto &info = m_sourceInfoMap[source];
      info.m_collectorClient = this;
      info.m_eventUpdateSignal.connect(controller, function);
    }

  }

} 

#endif  //  DQM4HEP_EVENTCOLLECTORCLIENT_H
