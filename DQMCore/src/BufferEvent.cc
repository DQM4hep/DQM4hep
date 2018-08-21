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
#include "dqm4hep/BufferEvent.h"
#include "dqm4hep/PluginManager.h"

namespace dqm4hep {

  namespace core {
    
    BufferEvent::~BufferEvent() {
      clear();
    }
    
    //-------------------------------------------------------------------------------------------------

    EventPtr BufferEvent::make_shared() {
      auto ptr = std::shared_ptr<Event>(new EventBase<BufferEvent>(new BufferEvent()));
      ptr->setStreamerName("BufferEventStreamer");
      return ptr;
    }

    //-------------------------------------------------------------------------------------------------

    void BufferEvent::copyBuffer(const char *const b, std::size_t len) {
      if(nullptr == b) {
        return;
      }
      clear();
      m_buffer.m_size = len;
      m_buffer.m_buffer = new char[len];
      m_buffer.m_isOwner = true;
      memcpy(m_buffer.m_buffer, b, len);
    }
    
    //-------------------------------------------------------------------------------------------------

    void BufferEvent::moveBuffer(char *b, std::size_t len) {
      if(nullptr == b) {
        return;
      }
      clear();
      m_buffer.m_size = len;
      m_buffer.m_buffer = b;
      m_buffer.m_isOwner = true;
    }
    
    //-------------------------------------------------------------------------------------------------

    void BufferEvent::handleBuffer(char *b, std::size_t len) {
      if(nullptr == b) {
        return;
      }
      clear();
      m_buffer.m_size = len;
      m_buffer.m_buffer = b;
      m_buffer.m_isOwner = false;
    }
    
    //-------------------------------------------------------------------------------------------------

    const char* BufferEvent::buffer() const {
      return m_buffer.m_buffer;
    }
    
    //-------------------------------------------------------------------------------------------------

    std::size_t BufferEvent::bufferSize() const {
      return m_buffer.m_size;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void BufferEvent::clear() {
      if( nullptr != m_buffer.m_buffer && m_buffer.m_isOwner ) {
        delete [] m_buffer.m_buffer;
      }
      m_buffer.m_buffer = nullptr;
      m_buffer.m_size = 0;
    }

  }
  
}
