/// \file BufferEvent.h
/*
 *
 * BufferEvent.h header template automatically generated by a class generator
 * Creation date : lun. mars 7 2016
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

#ifndef DQM4HEP_BUFFEREVENT_H
#define DQM4HEP_BUFFEREVENT_H

// -- dqm4hep headers
#include <dqm4hep/Event.h>
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/StatusCodes.h>

// -- xdrstream headers
#include <xdrstream/xdrstream.h>

namespace dqm4hep {

  namespace core {

    class BufferEventStreamer;

    /** 
     *  @brief  BufferEvent class
     */
    class BufferEvent {
      friend class BufferEventStreamer;
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
