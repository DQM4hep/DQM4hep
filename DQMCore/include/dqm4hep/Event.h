//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_EVENT_H
#define DQM4HEP_EVENT_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>

class TBuffer;

namespace dqm4hep {

  namespace core {
    
    class EventStreamer;

    /** EventType enumerator
     */
    enum EventType {
      UNKNOWN_EVENT = 0,
      RAW_DATA_EVENT = 1,
      RECONSTRUCTED_EVENT = 2,
      PHYSICS_EVENT = 3,
      CUSTOM_EVENT = 4
    };

    /** Event class.
     *
     *  Base event class used a main interface. Basic information on
     *  the event structure can be optionally provided by the user.
     *  For a real event wrapping, the user must implements the EventBase
     *  interface and provide by providing a real event implementation.
     *  Template methods of this class (i.e getEvent<T>()) refers to a
     *  BaseEvent<T> implementation.
     */
    class Event {
      friend class EventStreamer;
    public:
      /**
       *  @brief  Create an event pointer wrapping an event of type T
       *
       *  @param  args the arguments to pass to the T constructor (optional)
       */
      template <typename T, typename... Args>
      static EventPtr create(Args... args);
      
      /**
       *  @brief  Create an event pointer wrapping an event of type T
       *
       *  @param  event an already existing event to wrap
       */
      template <typename T>
      static EventPtr create(T *event);
      
      /** Destructor
       */
      virtual ~Event() = default;

      /** Set the event type
       */
      void setType(EventType type);

      /** Get the event type
       */
      EventType getType() const;

      /** Get the event source
       */
      void setSource(const std::string &sourceName);

      /** Set the event source
       */
      const std::string &getSource() const;

      /** Set the event time stamp
       */
      void setTimeStamp(const TimePoint &timeStamp);

      /** Get the event time stamp
       */
      const TimePoint &getTimeStamp() const;

      /** Get the real event implementation size,
       *  as extracted after its deserialization (unit bytes)
       */
      uint64_t getEventSize() const;

      /** Set the event number
       */
      void setEventNumber(uint32_t eventNumber);

      /** Get the event number
       */
      uint32_t getEventNumber() const;

      /** Set the run number
       */
      void setRunNumber(uint32_t runNumber);

      /** Get the run number
       */
      uint32_t getRunNumber() const;
      
      /** Set the streamer name
       */
      void setStreamerName(const std::string &streamer);
      
      /** Get the streamer name
       */
      const std::string &getStreamerName() const;

      /** Clear the event.
       *  Should call the real event implementation
       *  destructor if owned by the event wrapper
       */
      virtual void clear();

      /** Returns the true event implementation
       */
      template <typename T>
      T *getEvent() const;

      /** Set the event.
       *  Ownership transfered to event instance if isOwner argument set to true
       */
      template <typename T>
      void setEvent(T *pEvent, bool owner = true);
      
    protected:
      Event() = default;
      Event(const Event&) = delete;
      Event& operator=(const Event&) = delete;
      
      /** Set the real event implementation size,
       *  as extracted after its serialization (unit bytes)
       */
      void setEventSize(uint64_t eventSize);
      
      /** Write the base event information in the xdrstream device
       */
      void writeBase(TBuffer &buffer) const;

      /** Read the base event information from the xdrstream device
       */
      void readBase(TBuffer &buffer);

    protected:
      EventType              m_type = {UNKNOWN_EVENT};       ///< The event type
      std::string            m_source = {""};                ///< The event source
      TimePoint              m_timeStamp = {};               ///< The event time stamp
      uint64_t               m_eventSize = {0};              ///< The serialized event size (unit bytes)
      uint32_t               m_eventNumber = {0};            ///< The event number
      uint32_t               m_runNumber = {0};              ///< The run number
      std::string            m_streamerName = {""};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** EventBase interface
     */
    template <typename T>
    class EventBase : public Event {
    public:
      /** Constructor
       */
      EventBase();
      EventBase(const EventBase&) = delete;
      EventBase& operator=(const EventBase&) = delete;

      /** Constructor with real event
       */
      EventBase(T *pEvent);

      /** Destructor
       */
      ~EventBase() override;

      /** Returns the real event implementation
       */
      T *getEvent() const;

      /** Set the real event implementation
       */
      void setEvent(T *pEvent, bool owner = true);

      /** Whether the event wrapper owns the real event implementation
       */
      bool isOwner() const;

      /** The implementation of this method may delete the real event implementation if needed.
       *  ATTN : do not forget the check the event ownership using the isOwner() method
       */
      void clear() override;

    protected:
      T *m_pEvent = {nullptr};    ///< The real event implementation
      bool m_isOwner = {true}; ///< Whether the event wrapper owns the real event implementation
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *Event::getEvent() const {
      const EventBase<T> *const pEventBase(dynamic_cast<const EventBase<T> *const>(this));

      if (nullptr == pEventBase)
        return nullptr;

      return pEventBase->getEvent();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Event::setEvent(T *pEvent, bool owner) {
      EventBase<T> *const pEventBase(dynamic_cast<EventBase<T> *const>(this));
      if (NULL == pEventBase) {
        if (owner) {
          delete pEvent;
        }
        return;
      }
      pEventBase->clear();
      pEventBase->setEvent(pEvent, owner);
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setType(EventType type) {
      m_type = type;
    }

    //-------------------------------------------------------------------------------------------------

    inline EventType Event::getType() const {
      return m_type;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setSource(const std::string &sourceName) {
      m_source = sourceName;
    }

    //-------------------------------------------------------------------------------------------------

    inline const std::string &Event::getSource() const {
      return m_source;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setTimeStamp(const TimePoint &timeStamp) {
      m_timeStamp = timeStamp;
    }

    //-------------------------------------------------------------------------------------------------

    inline const TimePoint &Event::getTimeStamp() const {
      return m_timeStamp;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setEventSize(uint64_t eventSize) {
      m_eventSize = eventSize;
    }

    //-------------------------------------------------------------------------------------------------

    inline uint64_t Event::getEventSize() const {
      return m_eventSize;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setEventNumber(uint32_t eventNumber) {
      m_eventNumber = eventNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline uint32_t Event::getEventNumber() const {
      return m_eventNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Event::setRunNumber(uint32_t runNumber) {
      m_runNumber = runNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline uint32_t Event::getRunNumber() const {
      return m_runNumber;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline EventBase<T>::EventBase() : 
      Event() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline EventBase<T>::EventBase(T *pEvent) : 
      Event(), 
      m_pEvent(pEvent) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline EventBase<T>::~EventBase() {
      this->clear();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *EventBase<T>::getEvent() const {
      return m_pEvent;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void EventBase<T>::setEvent(T *pEvent, bool owner) {
      this->clear();
      m_pEvent = pEvent;
      m_isOwner = owner;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void EventBase<T>::clear() {
      Event::clear();

      if (nullptr != m_pEvent && this->isOwner())
        delete m_pEvent;

      m_pEvent = nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool EventBase<T>::isOwner() const {
      return m_isOwner;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename T, typename... Args>
    EventPtr Event::create(Args... args) {
      T *impl = nullptr;
      try {
        impl = new T(args...);
      }
      catch(...) {
        return nullptr;
      }
      return std::shared_ptr<Event>(new EventBase<T>(impl));
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    EventPtr Event::create(T *event) {
      return std::shared_ptr<Event>(new EventBase<T>(event));
    }
    
  }
  
}

#endif //  DQM4HEP_EVENT_H
