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
#include "dqm4hep/EventCollectorClient.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/OnlineRoutes.h"

namespace dqm4hep {

  namespace online {
    
    
    void EventCollectorClient::SourceInfo::receiveEvent(const net::Buffer &buffer) {
      core::EventPtr event = m_collectorClient->readEvent(buffer);
      m_eventUpdateSignal.emit(event);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    EventCollectorClient::EventCollectorClient(const std::string &collectorName) :
      m_collectorName(collectorName) {
        /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    core::EventPtr EventCollectorClient::queryEvent(const std::string &source) {
      core::EventPtr event = nullptr;
      net::Buffer buffer;
      auto model = buffer.createModel<std::string>();
      buffer.setModel(model);
      model->copy(source);
      m_client.sendRequest(
        OnlineRoutes::EventCollector::eventRequest(m_collectorName),
        buffer,
        [&event,this](const net::Buffer &response){
          event = this->readEvent(response);
      });
      return event;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollectorClient::startEventUpdates(const std::string &source) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      setUpdateMode(source, true);
    }

    //-------------------------------------------------------------------------------------------------

    void EventCollectorClient::stopEventUpdates(const std::string &source) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      setUpdateMode(source, false);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollectorClient::setUpdateMode(const std::string &source, bool receiveUpdates) {
      auto findIter = m_sourceInfoMap.find(source);
      if(m_sourceInfoMap.end() == findIter) {
        dqm_error( "EventCollectorClient::setUpdateMode: no source with '{0}' registered !", source );
        throw core::StatusCodeException(core::STATUS_CODE_NOT_FOUND);
      }
      bool subscribed = m_client.hasSubscribed(OnlineRoutes::EventCollector::eventUpdate(m_collectorName, source));
      
      // two cases:
      // 1) already subscribed and don't want to receive updates anymore
      // 2) not subscribed and want to receive updates
      if(subscribed != receiveUpdates) {
        if(receiveUpdates) {
          m_client.subscribe(
            OnlineRoutes::EventCollector::eventUpdate(m_collectorName, source),
            &findIter->second,
            &EventCollectorClient::SourceInfo::receiveEvent
          );          
        }
        else {
          m_client.unsubscribe(
            OnlineRoutes::EventCollector::eventUpdate(m_collectorName, source), 
            &findIter->second
          );
        }
      }
    }

    //-------------------------------------------------------------------------------------------------

    bool EventCollectorClient::receivingEventUpdates(const std::string &source) const {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto findIter = m_sourceInfoMap.find(source);
      if(m_sourceInfoMap.end() == findIter) {
        dqm_error( "EventCollectorClient::receivingEventUpdates: no source with '{0}' registered !", source );
        throw core::StatusCodeException(core::STATUS_CODE_NOT_FOUND);
      }
      return m_client.hasSubscribed(OnlineRoutes::EventCollector::eventUpdate(m_collectorName, source));
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollectorClient::startEventUpdates() {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      for(auto &source : m_sourceInfoMap) {
        setUpdateMode(source.first, true);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventCollectorClient::stopEventUpdates() {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      for(auto &source : m_sourceInfoMap) {
        setUpdateMode(source.first, false);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::EventPtr EventCollectorClient::readEvent(const net::Buffer &buffer) {
      std::lock_guard<std::recursive_mutex> lock(m_mutex);
      
      try {
        m_buffer.SetBuffer((void*)buffer.begin(), buffer.size(), false);      
      }
      catch(...) {
        dqm_error( "EventCollectorClient::readEvent: couldn't setup buffer device !" );
        return nullptr;
      }
      // read event using event streamer
      core::EventPtr event(nullptr);
      core::StatusCode statusCode = m_eventStreamer.readEvent(event, m_buffer);
      
      if(core::STATUS_CODE_SUCCESS != statusCode) {
        dqm_error( "EventCollectorClient::readEvent: streamer couldn't read event: {0}", core::statusCodeToString(statusCode) );
        return nullptr;
      }
      return event;
    }


  }

}
