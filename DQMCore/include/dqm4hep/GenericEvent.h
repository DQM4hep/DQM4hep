//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_GENERICEVENT_H
#define DQM4HEP_GENERICEVENT_H

// -- dqm4hep headers
#include <dqm4hep/Event.h>
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/StatusCodes.h>

namespace dqm4hep {

  namespace core {

    class GenericEventStreamer;

    /** GenericEvent class
     *
     *  Basic event implementation with a list of vectors stored in maps.
     *  See GenericEvent::setValues() and GenericEvent::getValues() to
     *  respectively set and get values into these map.
     *
     *  To get this kind of event within an analysis module, proceed like this :
     *
     *  \code
     *
     *  StatusCode MyModule::processEvent(Event *pEvent) {
     *
     *  	GenericEvent *pGenericEvent = pEvent->getEvent<GenericEvent>();
     *
     *      // Access contents via getValues()
     *  	FloatVector temperatures;
     *  	pGenericEvent->getValues("Temperature", temperatures);
     *  	// ...
     *  }
     *
     *  \endcode
     */
    class GenericEvent {
    public:
      /**
       *  @brief  Allocate a shared pointer of EventPtr
       */
      static EventPtr make_shared();
      
      /** Constructor
       */
      GenericEvent();

      /** Destructor
       */
      ~GenericEvent();

      /** Set a vector of values identified by key.
       *
       *  Attention : Template interface restricted to the following types :
       *    - vector<int>
       *    - vector<float>
       *    - vector<double>
       *    - vector<string>
       *  In case where an another parameter type is passed, the code will compile
       *  but will do nothing and return failure.
       */
      template <typename T>
      StatusCode setValues(const std::string &key, const T &vals);

      /** Get a vector of values identified by key.
       *
       *  Attention : Template interface restricted to the following types :
       *    - vector<int>
       *    - vector<float>
       *    - vector<double>
       *    - vector<string>
       *  In case where an another parameter type is passed, the code will compile
       *  but will do nothing and return failure.
       */
      template <typename T>
      StatusCode getValues(const std::string &key, T &vals) const;

    private:
      typedef std::map<std::string, IntVector> IntVectorMap;
      typedef std::map<std::string, FloatVector> FloatVectorMap;
      typedef std::map<std::string, DoubleVector> DoubleVectorMap;
      typedef std::map<std::string, StringVector> StringVectorMap;

      IntVectorMap m_intValues = {};
      FloatVectorMap m_floatValues = {};
      DoubleVectorMap m_doubleValues = {};
      StringVectorMap m_stringValues = {};

      friend class GenericEventStreamer;
    };

  }
  
}

#endif //  DQM4HEP_GENERICEVENT_H
