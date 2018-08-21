//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_EVENTSOURCE_H
#define DQM4HEP_EVENTSOURCE_H

// -- dqm4hep headers
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/Event.h>
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/Client.h>

// -- root headers
#include <TBufferFile.h>

namespace dqm4hep {

  namespace online {
    
    class EventSource;
    typedef std::shared_ptr<EventSource> EventSourcePtr;

    /** 
     *  @brief  EventSource class.
     *          Main interface for sending events to collectors (server process).
     *          Only one event source is allowed per process as the source is registered 
     *          using the network client id on the collector server side.
     *          It is possible to send events to multiple event collectors.
     *          A typical usage could be:
     *          @code
     *          EventSourcePtr source = EventSource::make_shared("EventBuilder");
     *          source->addCollector("CentralCollector");
     *          source->addCollector("PrivateCollector");
     *          source->start();
     *          // ...
     *          core::EventPtr event = getLastEvent(); // hypothetical getter function  
     *          source->sendEvent(event); // send to all collectors
     *          @endcode
     */
    class EventSource {
    public:
      /**
       *  @brief  Factory method to create a shared pointer of event source
       *  
       *  @param  name the source name
       */
      static EventSourcePtr make_shared(const std::string &name);

      /**
       *  @brief  Destructor
       */
      ~EventSource();

      /**
       *  @brief  Get the source name
       */
      const std::string &sourceName() const;
      
      /**
       *  @brief  Add a new collector server to which events will be sent.
       *          Can be used only before calling start().
       *          Must be called at least one time before calling start().
       *          
       *  @param  name the event collector name
       */
      void addCollector(const std::string &name);
      
      /**
       *  @brief  Start the event source.
       *          Setup raw buffers and register it to event collectors
       */
      void start();

      /**
       *  @brief  Send a single event to all registered collectors.
       *  
       *  @param  event the event pointer to serialize and send
       */
      void sendEvent(core::EventPtr event);
      
      /**
       *  @brief  Send a single event to a single collector. The collector must have been registered
       *          with the function addCollector() before calling this function
       *
       *  @param  collector the event collector to send 
       *  @param  event the event pointer to serialize and send
       */
      void sendEvent(const std::string &collector, core::EventPtr event);
      
    private:
      /**
       *  @brief  Get the source info (host info + source info)
       *  
       *  @param  info the json value to receive
       */
      void getSourceInfo(core::json &info);
      
      /**
       *  @brief  Register the event source to the specified collector
       *  
       *  @param  collector the collector name to register to
       *  @param  info the source info to send to the collector
       *  @return bool whether the source was registered
       */
      bool registerMe(const std::string &collector, const core::json &info);
      
      /**
       *  @brief  Un-register the event source from the specified collector
       *  
       *  @param  collector the collector name to un-register to
       */
      void unregisterMe(const std::string &collector);
      
      /**
       *  @brief  Perform the actual event sending to the specified list of collectors
       *  
       *  @param  collectors the list of collectors
       *  @param  event the event to send
       */
      void sendEvent(const core::StringVector &collectors, core::EventPtr event);

    private:
      /** 
       *  @brief  Constructor
       *
       *  @param  sourceName the source name
       */
      EventSource(const std::string &sourceName);
      
      /**
       *  @brief  CollectorInfo struct
       */
      struct CollectorInfo {
        bool             m_registered = {false};   ///< Whether the source is registered to the event collector
      };
      
    private:
      typedef std::map<std::string, CollectorInfo> CollectorInfoMap;
      
      bool                                m_started = {false};               ///< Whether the event source was started
      std::string                         m_sourceName = {""};               ///< The source name
      core::EventStreamer                 m_eventStreamer = {};              ///< The event streamer
      CollectorInfoMap                    m_collectorInfos = {};             ///< The map of event collector infos
      net::Client                         m_client = {};                     ///< The networking client interface 
      TBufferFile                         m_buffer = {TBuffer::kWrite, 2*1024*1024};  ///< The serialized event raw buffer
    };

  }

} 

#endif  //  DQM4HEP_EVENTSOURCE_H
