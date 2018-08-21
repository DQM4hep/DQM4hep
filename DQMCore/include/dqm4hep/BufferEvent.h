//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_BUFFEREVENT_H
#define DQM4HEP_BUFFEREVENT_H

// -- dqm4hep headers
#include <dqm4hep/Event.h>
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/StatusCodes.h>

namespace dqm4hep {

  namespace core {

    /** 
     *  @brief  BufferEvent class
     */
    class BufferEvent {
    private:
      /**
       *  @brief  Buffer struct. Internal buffer handler for the BufferEvent class
       */
      struct Buffer {
        /// The raw buffer pointer
        char *          m_buffer = {nullptr};
        /// The buffer size in memory
        std::size_t     m_size = {0};
        /// Whether the structure owns the buffer
        bool            m_isOwner = {true};
      };
      
    public:
      /**
       *  @brief  Allocate a shared pointer of EventPtr
       */
      static EventPtr make_shared();
      
      BufferEvent() = default;
      
      /**
       *  @brief  Destructor
       */
      ~BufferEvent();

      /**
       *  @brief  Copy the target buffer in the event structure using memcpy.
       *          The input buffer is still owned by the user after calling this method.
       *           
       * @param  b the buffer to copy
       * @param  len the buffer length
       */
      void copyBuffer(const char *const b, std::size_t len);
      
      /**
       *  @brief  Move the user buffer to the event structure.
       *          The buffer is not copied and is owned by the event after calling this method.
       *          
       * @param b the buffer to move
       * @param len the buffer length
       */
      void moveBuffer(char *b, std::size_t len);
      
      /**
       *  @brief  Let the event structure handle the buffer.
       *          The buffer is not copied and not owned by the event structure.
       *          ATN: This method is typically used for optimizing data transfer while sending
       *          event using an event source. As the handled buffer is still owned by the user, 
       *          it can be deleted by the user leading to an invalid state of the internal event
       *          buffer. To make sure the event is not in an invalid state after buffer deletion 
       *          in this case, use the method clear() right after the buffer deletion.
       *                
       * @param b the buffer to handle
       * @param len the buffer length
       */
      void handleBuffer(char *b, std::size_t len);
      
      /**
       *  @brief  Access the event internal buffer 
       */
      const char* buffer() const; 
      
      /**
       *  @brief  Access the event internal buffer length
       */
      std::size_t bufferSize() const;
      
      /**
       *  @brief  Clear the internal event buffer
       */
      void clear();

    private:
      /// The internal buffer handler
      Buffer            m_buffer = {};
    };

  }
  
}

#endif //  DQM4HEP_BUFFEREVENT_H
