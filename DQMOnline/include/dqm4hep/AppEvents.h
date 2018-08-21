//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_APPEVENTS_H
#define DQM4HEP_APPEVENTS_H

// -- dqm4hep headers
#include "dqm4hep/AppEvent.h"
#include "dqm4hep/Event.h"
#include "dqm4hep/Run.h"
#include "dqm4hep/DQMNet.h"

namespace dqm4hep {

  namespace online {
    
    class ServiceUpdateEvent : public AppEvent {
    public:
      ServiceUpdateEvent(const std::string &serviceName, net::BufferModelPtr bufferModel);
      const std::string &serviceName() const;
      const net::Buffer &buffer() const;
    private:
      std::string              m_serviceName = {""};
      net::Buffer              m_buffer = {};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    class RequestEvent : public AppEvent {
    public:
      RequestEvent(const std::string &name, net::BufferModelPtr requestBufferModel, net::Buffer &response);
      const std::string &requestName() const;
      const net::Buffer &request() const;
      net::Buffer &response() const;
    private:
      std::string              m_requestName = {""};
      net::Buffer              m_requestBuffer = {};
      net::Buffer             &m_responseBuffer;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    class CommandEvent : public AppEvent {
    public:
      CommandEvent(const std::string &commandName, net::BufferModelPtr bufferModel);
      const std::string &commandName() const;
      const net::Buffer &buffer() const;
    private:
      std::string              m_commandName = {""};
      net::Buffer              m_buffer = {};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    class StoreEvent : public AppEvent {
    public:
      StoreEvent(int type, T data);
      const T &data() const;
    private:
      T                m_data;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline ServiceUpdateEvent::ServiceUpdateEvent(const std::string &sname, net::BufferModelPtr bufferModel) :
      AppEvent(SERVICE_UPDATE),
      m_serviceName(sname) {
      m_buffer.setModel(bufferModel);
    }
    
    //-------------------------------------------------------------------------------------------------

    inline const std::string &ServiceUpdateEvent::serviceName() const {
      return m_serviceName;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline const net::Buffer &ServiceUpdateEvent::buffer() const {
      return m_buffer;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline RequestEvent::RequestEvent(const std::string &name, net::BufferModelPtr requestBufferModel, net::Buffer &responseBuffer) :
      AppEvent(AppEvent::REQUEST_HANDLING),
      m_requestName(name),
      m_responseBuffer(responseBuffer) {
      m_requestBuffer.setModel(requestBufferModel);
    }
    
    //-------------------------------------------------------------------------------------------------

    inline const std::string &RequestEvent::requestName() const {
      return m_requestName;
    }

    //-------------------------------------------------------------------------------------------------

    inline const net::Buffer &RequestEvent::request() const {
      return m_requestBuffer;
    }
    
    //-------------------------------------------------------------------------------------------------

    inline net::Buffer &RequestEvent::response() const {
      return m_responseBuffer;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    inline CommandEvent::CommandEvent(const std::string &cname, net::BufferModelPtr bufferModel) :
      AppEvent(AppEvent::COMMAND_HANDLING),
      m_commandName(cname) {
      m_buffer.setModel(bufferModel);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline const std::string &CommandEvent::commandName() const {
      return m_commandName;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline const net::Buffer &CommandEvent::buffer() const {
      return m_buffer;
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline StoreEvent<T>::StoreEvent(int t, T d) :
      AppEvent(t),
      m_data(d) {
      /* nop */
    }
    
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline const T &StoreEvent<T>::data() const {
      return m_data;
    }

  }

} 

#endif  //  DQM4HEP_APPEVENTS_H
