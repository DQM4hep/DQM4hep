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
#include "dqm4hep/AppEventLoop.h"
#include "dqm4hep/Logging.h"

// -- std headers
#include <memory>

namespace dqm4hep {

  namespace online {
        
    AppEventLoop::AppEventLoop() {
      
    }
    
    //-------------------------------------------------------------------------------------------------
    
    AppEventLoop::~AppEventLoop() {
      
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::postEvent(AppEvent *pAppEvent) {
      if(nullptr == pAppEvent) {
        return;
      }
      // push event in the queue
      std::lock_guard<std::recursive_mutex> lock(m_queueMutex);      
      m_eventQueue.insert(pAppEvent);   
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::sendEvent(AppEvent *pAppEvent) {
      if(nullptr == pAppEvent) {
        return;
      }
      // process an event
      try {
        this->processEvent(pAppEvent);        
      }
      catch(...) {
        dqm_error( "AppEventLoop::sendEvent: caught exception for event {0} of type {1}", (void*)pAppEvent, pAppEvent->type() );
      }
      delete pAppEvent;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::clear() {
      std::lock_guard<std::recursive_mutex> lock(m_queueMutex);
      for(auto evt : m_eventQueue) {
        delete evt;
      }
      m_eventQueue.clear();
    }
    
    //-------------------------------------------------------------------------------------------------

    int AppEventLoop::exec() {
      m_running = true;
      m_quitFlag = false;
      
      m_timerThread = std::thread(&AppEventLoop::timerThread, this);
      
      while(1) {
        if(m_quitFlag) {
          break;
        }
        
        // safely get the app event pointer 
        AppEvent* event = nullptr;
        
        {
          std::lock_guard<std::recursive_mutex> lock(m_queueMutex);
          
          if(!m_eventQueue.empty()) {
            event = *m_eventQueue.rbegin();
            m_eventQueue.erase(event);               
          }
        }
        
        // if no event, save cpu ressources ...
        if(nullptr == event) {
          usleep(100);
          continue;
        }
        
        try {
          this->processEvent(event);
        }
        catch(...) {
          m_returnCode = 1;
          break;
        }
        delete event;
        usleep(100);
      }
      
      m_timerStopFlag = true;
      m_timerThread.join();      
      m_running = false;
      return m_returnCode.load();
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool AppEventLoop::running() const {
      return m_running.load();
    }

    //-------------------------------------------------------------------------------------------------    
    
    void AppEventLoop::exit(int returnCode) {
      auto event = new StoreEvent<int>(AppEvent::QUIT, returnCode);
      event->setPriority(100);
      this->sendEvent(event);
    }

    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::quit() {
      this->exit(0);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::processEvent(AppEvent *pAppEvent) {    
      try {
        std::lock_guard<std::recursive_mutex> lock(m_eventMutex);
        m_onEventSignal.emit(pAppEvent);
      }
      catch(core::StatusCodeException &except) {
        std::lock_guard<std::recursive_mutex> lock(m_exceptionMutex);
        
        if(!m_onExceptionSignal.hasConnection()) {
          dqm_error( "EventLoop::exec(): Caught exception: {0}" , except.what() );
          throw except;          
        }
        else {
          m_onExceptionSignal.emit(pAppEvent);
        }
      }
      catch(std::exception &except) {
        std::lock_guard<std::recursive_mutex> lock(m_exceptionMutex);
        
        if(!m_onExceptionSignal.hasConnection()) {
          dqm_error( "EventLoop::exec(): Caught exception: {0}" , except.what() );
          throw except;          
        }
        else {
          m_onExceptionSignal.emit(pAppEvent);
        }
      }
      catch(...) {
        std::lock_guard<std::recursive_mutex> lock(m_exceptionMutex);
        
        if(!m_onExceptionSignal.hasConnection()) {
          dqm_error( "EventLoop::exec(): Caught unknown exception !" );
          throw core::StatusCodeException(core::STATUS_CODE_FAILURE);
        }
        else {
          m_onExceptionSignal.emit(pAppEvent);
        }
      }
      
      if(pAppEvent->type() == AppEvent::QUIT) {
        m_quitFlag = true;
        m_returnCode = 1;
        StoreEvent<int> *quitEvent = dynamic_cast<StoreEvent<int>*>(pAppEvent);
        
        if(quitEvent) {
          m_returnCode = quitEvent->data();
        }
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    int AppEventLoop::count(int eventType) {
      std::lock_guard<std::recursive_mutex> lock(m_queueMutex);
      return std::count_if(m_eventQueue.begin(), m_eventQueue.end(), [&eventType](AppEvent* ptr){
        return (ptr->type() == eventType);
      });
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::timerThread() {
      {
        // initialize start time point of all timers
        std::lock_guard<std::recursive_mutex> lock(m_timerMutex);
        for(auto timer : m_timers) {
          timer->m_startTime = core::now();
        }
      }
      while(not m_timerStopFlag.load()) {
        {
          std::lock_guard<std::recursive_mutex> lock(m_timerMutex);
          for(auto timer : m_timers) {
            if(timer->active()) {
              const unsigned int timeoutEllapsed = std::chrono::duration_cast<std::chrono::milliseconds>(core::now()-timer->m_startTime).count();
              const bool timeoutReached = (timeoutEllapsed >= timer->interval());
              if(timeoutReached) {
                // process timer timeout in event loop
                processFunction([timer](){
                  timer->m_signal.emit();
                });
                // stop it if single shot
                if(timer->singleShot()) {
                  timer->m_active = false;
                }
                else {
                  timer->m_startTime = core::now();
                }
              }
            }
            if(m_timerStopFlag.load()) {
              break;
            }
          }
        }
        usleep(100);
      }
      dqm_debug( "Exiting timer thread !" );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::addTimer(AppTimer *timer) {
      std::lock_guard<std::recursive_mutex> lock(m_timerMutex);
      m_timers.insert(timer);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::startTimer(AppTimer *timer) {
      std::lock_guard<std::recursive_mutex> lock(m_timerMutex);
      timer->m_startTime = core::now();
      timer->m_active = true;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::stopTimer(AppTimer *timer) {
      std::lock_guard<std::recursive_mutex> lock(m_timerMutex);
      auto findIter = m_timers.find(timer);
      if(m_timers.end() != findIter) {
        (*findIter)->m_active = false;
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void AppEventLoop::removeTimer(AppTimer *timer) {
      stopTimer(timer);
      std::lock_guard<std::recursive_mutex> lock(m_timerMutex);
      m_timers.erase(timer);
      delete timer;
    }
    
  }

}

