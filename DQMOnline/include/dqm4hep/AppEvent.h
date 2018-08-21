//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_APPEVENT_H
#define DQM4HEP_APPEVENT_H

namespace dqm4hep {

  namespace online {

    class AppEvent {
    public:
      /**
       *  @brief  AppEvent type enum
       */
      enum Type {
        NONE = 0,
        QUIT,
        SERVICE_UPDATE,
        REQUEST_HANDLING,
        COMMAND_HANDLING,
        CLIENT_EXIT,
        START_OF_RUN,
        END_OF_RUN,
        END_OF_CYCLE,
        PROCESS_EVENT,
        USER = 1024,
        MAX_USER = 65535
      };
      
      /**
       *  @brief  Constructor
       */
      AppEvent(int type);
      
      /**
       *  @brief  Destructor
       */
      virtual ~AppEvent();
      
      /**
       *  @brief  Returns the event type
       */
      int type() const;
      
      /**
       *  @brief  Set the event priority, range [0,100]
       *  
       *  @param  priority the event priority
       */
      void setPriority(int priority);
      
      /**
       *  @brief  Get the event priority
       */
      int priority() const;
      
    private:
      /// The application event type
      int               m_type = {AppEvent::NONE};
      /// The event priority
      int               m_priority = {50};
    };

  }

} 

#endif  //  DQM4HEP_APPEVENT_H
