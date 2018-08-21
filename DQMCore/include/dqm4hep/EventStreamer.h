//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_EVENTSTREAMER_H
#define DQM4HEP_EVENTSTREAMER_H

// -- dqm4hep headers
#include <dqm4hep/Event.h>
#include <dqm4hep/StatusCodes.h>

class TBuffer;

namespace dqm4hep {

  namespace core {

    class Event;
    
    class EventStreamer {
    public:
      /**
       *  @brief  Default constructor
       */
      EventStreamer() = default;
      
      /**
       *  @brief  Default destructor
       */
      ~EventStreamer() = default;
      
      /**
       *  @brief  Write an event using an xdrstream device.
       *          The streamer info is taken from Event::getStreamerName()
       *          and from the plugin manager.
       *          
       *  @param  event the event to write
       *  @param  buffer the buffer to write with
       */
      StatusCode writeEvent(EventPtr event, TBuffer &buffer);
      
      /**
       *  @brief  Read an event using an xdrstream device.
       *          The streamer info is read from the buffer
       *          and the streamer if allocated from the plugin manager.
       *          
       *  @param  event the event to read
       *  @param  buffer the buffer to read with
       */
      StatusCode readEvent(EventPtr &event, TBuffer &buffer);
      
    private:
      std::string                  m_streamerName = {""};     ///< The current event streamer name in use
      EventStreamerPluginPtr       m_streamer = {nullptr};    ///< The current event streamer in use
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** EventStreamerPlugin class
     */
    class EventStreamerPlugin {
    public:
      /** Destructor
       */
      virtual ~EventStreamerPlugin() = default;

      /** Factory method to create the corresponding event to this streamer.
       *  The event is expected to contains an allocated wrapped event
       */
      virtual EventPtr createEvent() const = 0;

      /** Serialize an event object and store it in the data stream
       */
      virtual StatusCode write(EventPtr event, TBuffer &buffer) = 0;

      /** De-serialize an event given from the data stream
       */
      virtual StatusCode read(EventPtr event, TBuffer &buffer) = 0;
    };

  }
  
}

#endif //  DQM4HEP_EVENTSTREAMER_H
