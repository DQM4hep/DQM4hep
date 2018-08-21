//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_EVENTREADER_H
#define DQM4HEP_EVENTREADER_H

// -- dqm4hep headers
#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/Run.h"
#include "dqm4hep/Event.h"
#include "dqm4hep/Signal.h"

namespace dqm4hep {

  namespace core {
    
    /**
     *  @brief  EventReader class
     *
     *  Implement the logic of reading a file containing events.
     *  While reading the file, the daughter class has to use the
     *  available signal for notifying new events.
     *
     *  Typical use of event reader:
     *  @code{.cpp}
     *  // user custom class that cosume the data
     *  MyConsumer consumer;
     *  EventReader *reader = new MyFileReader();
     *  reader->onNewRun().connect(&consumer, &MyConsumer::processNewRun);
     *  reader->onEventRead().connect(&consumer, &MyConsumer::processEvent);
     *  reader->openFile("detector_I12548.raw");
     *  // skip 5 first events (not mandatory)
     *  reader->skipNEvents(5);
     *  while(STATUS_CODE_SUCCESS == reader->readNextEvent());
     *  @endcode
     */
    class EventReader {
    public:
      /**
       *  @brief  Destructor
       */
      virtual ~EventReader();
      
      /**
       *  @brief  Open a new file
       *
       *  @param  fname the file name to open
       */
      virtual core::StatusCode open(const std::string &fname) = 0;
      
      /**
       *  @brief  Skip n events from the current position (usually from beginning of file)
       *  
       *  @param  nEvents the number of event to skip
       */
      virtual core::StatusCode skipNEvents(int nEvents) = 0;
      
      /**
       *  @brief  Extract the run info from file
       * 
       *  @param  run the run to receive
       */
      virtual core::StatusCode runInfo(core::Run &run) = 0;
      
      /**
       *  @brief  Read next event in the file and post it using the signal.
       *          STATUS_CODE_OUT_OF_RANGE is synonym of end of file.
       *          Anything else than STATUS_CODE_SUCCESS is considered as an error
       */
      virtual core::StatusCode readNextEvent() = 0;
      
      /**
       *  @brief  Close the current file
       */
      virtual core::StatusCode close() = 0;
      
      /**
       *  @brief  Get the signal on event read
       */
      core::Signal<core::EventPtr> &onEventRead();
      
    protected:
      /// The signal on event read
      core::Signal<core::EventPtr>              m_onEventRead = {};
    };
    
    typedef std::shared_ptr<EventReader> EventReaderPtr;
  }

} 

#endif  //  DQM4HEP_EVENTREADER_H
