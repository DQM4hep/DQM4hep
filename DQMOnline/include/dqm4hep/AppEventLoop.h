//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_APPEVENTLOOP_H
#define DQM4HEP_APPEVENTLOOP_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/AppEvent.h"
#include "dqm4hep/AppEvents.h"
#include "dqm4hep/Signal.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/AppTimer.h"

#include <mutex>
#include <atomic>

namespace dqm4hep {

  namespace online {
    
    class AppEventLoop {
      friend class AppTimer;
      friend class Application;
    public:     
      /**
       *  @brief  Constructor
       */
      AppEventLoop();
      
      /**
       *  @brief  Destructor
       */
      ~AppEventLoop();
       
      /**
       *  @brief  Post an event in the event queue. 
       *          The event queue is sorted by event priority.
       *          The event pointer ownership is taken by the event loop.
       *          
       *  @param  pAppEvent the event to post
       */
      void postEvent(AppEvent *pAppEvent);
      
      /**
       *  @brief  Send an event, to be directly processed.
       *          The event pointer ownership is taken by the event loop.
       *          
       *  @param  pAppEvent the event to send
       */
      void sendEvent(AppEvent *pAppEvent);
      
      /**
       *  @brief  Safely process the function in the event loop 
       *
       *  @param  function the function to process
       *  @param  args the function arguments
       */
      template <typename Function, typename... Args>
      void processFunction(Function function, Args ...args);
      
      /**
       *  @brief  Clear the event queue
       */
      void clear();

      /**
       *  @brief  Start processing the events in the event queue.
       *          This is a blocking function. To exit the event loop,
       *          post or send a QuitEvent event.
       *          Posting or sending events is thread safe, even when
       *          an event is being processed. 
       */
      int exec();
      
      /**
       *  @brief  Whether the event loop is running
       */
      bool running() const;
      
      /**
       *  @brief  Connect a function to callback on event processing.
       *          Calling this function is thread safe.
       *          Returns true if the connection has been made. 
       */
      template <typename T>
      bool connectOnEvent(T *pObject, void (T::*function)(AppEvent *));
      
      /**
       *  @brief  Disconnect all functions of the specified object for event processing.
       *          Calling this function is thread safe.
       *          Returns true if all connections has been removed. 
       */
      template <typename T>
      bool disconnectOnEvent(T *pObject);
      
      /**
       *  @brief  Whether the specified object has at least one connection for event processing.
       *          Calling this function is thread safe.
       */
      template <typename T>
      bool hasEventConnection(T *pObject);
      
      /**
       *  @brief  Exit the event loop with the specified status
       *
       *  @param  returnCode the code to return
       */
      void exit(int returnCode);
      
      /**
       *  @brief  Exit the event loop with a return code 0.
       */
      void quit();
      
      /**
       *  @brief  Add an exception handler function.
       *          If no handler is specified and an exception is caught,
       *          the event loop exit imediately by throwing an exception.
       *          The event causing the exception is passed to the callback function.
       *
       *  @param  pObject the object handling the exception
       *  @param  function the method called on exception
       */
      template <typename T>
      void onException(T *pObject, void (T::*function)(AppEvent *));
      
      /**
       *  @brief  Count the number of events of the given type currently in the event queue
       *  
       *  @param  eventType the event type
       */
      int count(int eventType);
      
      /**
       *  @brief  Count the number of events currently in the event queue satifying the unary predicate 
       *  
       *  @param  predicate the unary predicate
       */
      template <typename Predicate>
      int count(Predicate predicate);
      
    private:
      void processEvent(AppEvent *pAppEvent);
      
      void timerThread();
      void addTimer(AppTimer *timer);
      void startTimer(AppTimer *timer);
      void stopTimer(AppTimer *timer);
      void removeTimer(AppTimer *timer);
      
    private:
      // not copiable, not movable
      AppEventLoop &operator=(const AppEventLoop&) = delete;
      AppEventLoop(const AppEventLoop&) = delete;
      AppEventLoop(AppEventLoop&&) = delete;
      
      struct QueueCompare {
        bool operator()(AppEvent* lhs, AppEvent* rhs) const {
          return lhs->priority() < rhs->priority();
        }
      };
      
      std::set<AppEvent*,QueueCompare>             m_eventQueue = {};
      std::recursive_mutex                         m_queueMutex = {};
      std::recursive_mutex                         m_eventMutex = {};
      std::recursive_mutex                         m_exceptionMutex = {};
      std::recursive_mutex                         m_timerMutex = {};
      core::Signal<AppEvent *>                     m_onEventSignal = {};
      core::Signal<AppEvent *>                     m_onExceptionSignal = {};
      std::atomic<bool>                            m_running = {false};
      std::atomic<bool>                            m_quitFlag = {false};
      std::atomic<int>                             m_returnCode = {0};
      std::thread                                  m_timerThread = {};
      std::atomic_bool                             m_timerStopFlag = {false};
      std::set<AppTimer*>                          m_timers = {};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline bool AppEventLoop::connectOnEvent(T *pObject, void (T::*function)(AppEvent *)) {
      std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
      return m_onEventSignal.connect(pObject, function);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline bool AppEventLoop::disconnectOnEvent(T *pObject) {
      std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
      return m_onEventSignal.disconnect(pObject);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline bool AppEventLoop::hasEventConnection(T *pObject) {
      std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
      return m_onEventSignal.isConnected(pObject);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline void AppEventLoop::onException(T *pObject, void (T::*function)(AppEvent *)) {
      std::lock_guard<std::recursive_mutex> lock(m_exceptionMutex);
      return m_onExceptionSignal.connect(pObject, function);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename Predicate>
    inline int AppEventLoop::count(Predicate predicate) {
      std::lock_guard<std::recursive_mutex> lock(m_queueMutex);
      return std::count_if(m_eventQueue.begin(), m_eventQueue.begin(), predicate);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename Function, typename... Args>
    inline void AppEventLoop::processFunction(Function function, Args ...args) {
      std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
      function(args...);
    }

  }

} 

#endif  //  DQM4HEP_APPEVENTLOOP_H
