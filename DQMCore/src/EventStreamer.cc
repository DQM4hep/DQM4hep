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
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/Event.h>
#include <dqm4hep/PluginManager.h>

// -- root headers
#include <TBuffer.h>

namespace dqm4hep {

  namespace core {
    
    StatusCode EventStreamer::writeEvent(EventPtr event, TBuffer &buffer) {
      // consistency check
      if (nullptr == event) {
        return STATUS_CODE_INVALID_PARAMETER;
      }
      if(not buffer.IsWriting()) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      // setup event streamer
      std::string streamerName = event->getStreamerName();
      if(not m_streamer or m_streamerName != streamerName) {
        m_streamer = nullptr;
        m_streamer = PluginManager::instance()->create<EventStreamerPlugin>(streamerName);
        if(not m_streamer) {
          dqm_error( "EventStreamer::writeEvent: streamer '{0}' not registered in plugin manager !", streamerName );
          return STATUS_CODE_FAILURE;
        }
        m_streamerName = streamerName;
      }
      // write streamer name
      buffer.WriteStdString(&m_streamerName);
      // write base event data
      event->writeBase(buffer);
      // write user event data
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_streamer->write(event, buffer));
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    StatusCode EventStreamer::readEvent(EventPtr &event, TBuffer &buffer) {
      // consistency check
      if (not buffer.IsReading()) {
        return STATUS_CODE_NOT_ALLOWED;
      }
      // read streamer name
      std::string streamerName;
      buffer.ReadStdString(&streamerName);
      // setup event streamer
      if(not m_streamer or m_streamerName != streamerName) {
        m_streamer = nullptr;
        m_streamer = PluginManager::instance()->create<EventStreamerPlugin>(streamerName);
        if(not m_streamer) {
          dqm_error( "EventStreamer::readEvent: streamer '{0}' not registered in plugin manager !", streamerName );
          return STATUS_CODE_FAILURE;
        }
        m_streamerName = streamerName;
      }
      // read user event data
      event = m_streamer->createEvent();
      event->setStreamerName(m_streamerName);
      Int_t eventSize = buffer.Length();
      // write base event data
      event->readBase(buffer);
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_streamer->read(event, buffer));
      eventSize = buffer.Length() - eventSize;
      event->setEventSize(eventSize);
      return STATUS_CODE_SUCCESS;
    }
    
  }
  
}
