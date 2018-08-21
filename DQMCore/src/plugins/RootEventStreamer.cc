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
#include <dqm4hep/PluginManager.h>

// -- root header
#include <TObject.h>
#include <TBuffer.h>

namespace dqm4hep {

  namespace core {

    /**
     *  @brief  RootEventStreamer class
     */
    class RootEventStreamer : public EventStreamerPlugin {
    public:
      /** 
       *  @brief  Constructor
       */
      RootEventStreamer();

      /** 
       *  @brief  Destructor
       */
      ~RootEventStreamer() override;

      /** 
       *  @brief  Factory method to create the corresponding event to this streamer.
       *          The event is expected to contains an allocated wrapped event
       */
      EventPtr createEvent() const override;

      /** 
       *  @brief  Serialize the event and store it into a data stream.
       *
       *  @param  event the event to write out
       *  @param  device the xdrstream device to write with
       */
      StatusCode write(EventPtr event, TBuffer &buffer) override;

      /** 
       *  @brief  De-serialize the event.
       *
       *  @param  event the event to read in
       *  @param  device the xdrstream device to read from
       */
      StatusCode read(EventPtr event, TBuffer &buffer) override;
      
    private:
      ///< The ROOT streamer to read/write TObject objects
      // TBufferFile                m_buffer = {TBuffer::kRead};  
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    RootEventStreamer::RootEventStreamer() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    RootEventStreamer::~RootEventStreamer() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    EventPtr RootEventStreamer::createEvent() const {
      return std::shared_ptr<Event>(new EventBase<TObject>());
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode RootEventStreamer::write(EventPtr event, TBuffer &buffer) {
      const TObject *pTObject = event->getEvent<TObject>();
      if (nullptr == pTObject) {
        return STATUS_CODE_INVALID_PARAMETER;
      }
      buffer.WriteObject(pTObject);
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode RootEventStreamer::read(EventPtr event, TBuffer &buffer) {
      TObject *pTObject = buffer.ReadObject(0);
      if(nullptr == pTObject) {
        dqm_error( "RootEventStreamer::read: Couldn't read TObject, got nullptr" );
        return STATUS_CODE_FAILURE;
      }  
      event->setEvent<TObject>(pTObject, true);
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    DQM_PLUGIN_DECL(RootEventStreamer, "RootEventStreamer");
  }
  
}
