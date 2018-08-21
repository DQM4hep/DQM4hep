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
#include <dqm4hep/GenericEvent.h>
#include <dqm4hep/PluginManager.h>

// -- root headers
#include <TBuffer.h>

namespace dqm4hep {

  namespace core {

    /**
     * @brief GenericEventStreamer class
     */
    class GenericEventStreamer : public EventStreamerPlugin {
    public:
      /** Constructor
       */
      GenericEventStreamer();

      /** Destructor
       */
      ~GenericEventStreamer() override;

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
      
    private:
      template <typename T>
      void writeMap(TBuffer &buffer, const std::map<std::string, std::vector<T>> &values);
      
      template <typename T>
      void readMap(TBuffer &buffer, std::map<std::string, std::vector<T>> &values);
    };
    
    
    template <typename T>
    inline void GenericEventStreamer::writeMap(TBuffer &buffer, const std::map<std::string, std::vector<T>> &values) {
      buffer.WriteInt(values.size());
      for(auto elt : values) {
        buffer.WriteStdString(&elt.first);
        buffer.WriteArray(&elt.second[0], elt.second.size());
      }
    }
    
    template <typename T>
    inline void GenericEventStreamer::readMap(TBuffer &buffer, std::map<std::string, std::vector<T>> &values) {
      Int_t size = 0;
      buffer.ReadInt(size);
      for(Int_t s=0 ; s<size ; s++) {
        std::string key;
        buffer.ReadStdString(&key);
        T *array = nullptr;
        Int_t size = buffer.ReadArray(array);
        std::vector<T> vecValues(array, array + sizeof(array) / sizeof(T));
        delete [] array;
        values[key] = vecValues;
      }
    }
    
    template <>
    inline void GenericEventStreamer::writeMap(TBuffer &buffer, const std::map<std::string, std::vector<std::string>> &values) {
      buffer.WriteInt(values.size());
      for(auto elt : values) {
        buffer.WriteStdString(&elt.first);
        buffer.WriteInt(elt.second.size());
        for(auto elt2 : elt.second) {
          buffer.WriteStdString(&elt2);
        }
      }
    }
    
    template <>
    inline void GenericEventStreamer::readMap(TBuffer &buffer, std::map<std::string, std::vector<std::string>> &values) {
      Int_t size = 0;
      buffer.ReadInt(size);
      for(Int_t s=0 ; s<size ; s++) {
        std::string key;
        buffer.ReadStdString(&key);
        Int_t vecSize = 0;
        buffer.ReadInt(vecSize);
        std::vector<std::string> vecValues;
        vecValues.reserve(vecSize);
        for(Int_t s2=0 ; s2<vecSize ; s2++) {
          std::string value;
          buffer.ReadStdString(&value);
          vecValues.push_back(value);
        }
        values[key] = vecValues;
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    GenericEventStreamer::GenericEventStreamer() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    GenericEventStreamer::~GenericEventStreamer() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    EventPtr GenericEventStreamer::createEvent() const {
      return Event::create<GenericEvent>();
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode GenericEventStreamer::write(EventPtr event, TBuffer &buffer) {
      const GenericEvent *pGenericEvent = event->getEvent<GenericEvent>();
      if (nullptr == pGenericEvent) {
        return STATUS_CODE_INVALID_PARAMETER;
      }
      // write event contents
      writeMap(buffer, pGenericEvent->m_intValues);
      writeMap(buffer, pGenericEvent->m_floatValues);
      writeMap(buffer, pGenericEvent->m_doubleValues);
      writeMap(buffer, pGenericEvent->m_stringValues);
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode GenericEventStreamer::read(EventPtr event, TBuffer &buffer) {
      GenericEvent *pGenericEvent = event->getEvent<GenericEvent>();
      if (nullptr == pGenericEvent) {
        return STATUS_CODE_INVALID_PARAMETER;
      }
      // write event contents
      readMap(buffer, pGenericEvent->m_intValues);
      readMap(buffer, pGenericEvent->m_floatValues);
      readMap(buffer, pGenericEvent->m_doubleValues);
      readMap(buffer, pGenericEvent->m_stringValues);
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    DQM_PLUGIN_DECL(GenericEventStreamer, "GenericEventStreamer");
  }
}
