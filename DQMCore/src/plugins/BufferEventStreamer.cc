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
#include <dqm4hep/Event.h>
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/BufferEvent.h>
#include <dqm4hep/PluginManager.h>

// -- root headers
#include <TBuffer.h>

namespace dqm4hep {

  namespace core {

    /**
     * @brief BufferEventStreamer class
     */
    class BufferEventStreamer : public EventStreamerPlugin {
    public:
      BufferEventStreamer() = default;
      ~BufferEventStreamer() override = default;

      /** Factory method to create the corresponding event to this streamer.
       *  The event is expected to contains an allocated wrapped event
       */
      EventPtr createEvent() const override;

      /** Serialize the event and store it into a data stream.
       */
      StatusCode write(EventPtr event, TBuffer &buffer) override;

      /** De-serialize the event.
       */
      StatusCode read(EventPtr event, TBuffer &buffer) override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    EventPtr BufferEventStreamer::createEvent() const {
      return Event::create<BufferEvent>();
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode BufferEventStreamer::write(EventPtr event, TBuffer &buffer) {
      const BufferEvent *bufferEvent = event->getEvent<BufferEvent>();
      if (nullptr == bufferEvent) {
        return STATUS_CODE_INVALID_PARAMETER;
      }
      buffer.WriteArray(bufferEvent->buffer(), bufferEvent->bufferSize());
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode BufferEventStreamer::read(EventPtr event, TBuffer &buffer) {
      BufferEvent *bufferEvent = event->getEvent<BufferEvent>();      
      Char_t *rawBuffer = nullptr;
      Int_t size = buffer.ReadArray(rawBuffer);
      bufferEvent->moveBuffer(rawBuffer, size);
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    DQM_PLUGIN_DECL(BufferEventStreamer, "BufferEventStreamer");
  }
}
