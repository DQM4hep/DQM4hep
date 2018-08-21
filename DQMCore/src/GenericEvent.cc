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
#include "dqm4hep/GenericEvent.h"
#include "dqm4hep/PluginManager.h"

namespace dqm4hep {

  namespace core {

    EventPtr GenericEvent::make_shared() {
      auto ptr = std::shared_ptr<Event>(new EventBase<GenericEvent>(new GenericEvent()));
      ptr->setStreamerName("GenericEventStreamer");
      return ptr;
    }
    
    //-------------------------------------------------------------------------------------------------

    GenericEvent::GenericEvent() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    GenericEvent::~GenericEvent() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    StatusCode GenericEvent::setValues(const std::string &/*key*/, const T &/*vals*/) {
      return STATUS_CODE_FAILURE;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    StatusCode GenericEvent::getValues(const std::string &/*key*/, T &/*vals*/) const {
      return STATUS_CODE_FAILURE;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    StatusCode GenericEvent::setValues(const std::string &key, const IntVector &vals) {
      m_intValues[key] = vals;
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    StatusCode GenericEvent::setValues(const std::string &key, const FloatVector &vals) {
      m_floatValues[key] = vals;
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    StatusCode GenericEvent::setValues(const std::string &key, const DoubleVector &vals) {
      m_doubleValues[key] = vals;
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    StatusCode GenericEvent::setValues(const std::string &key, const StringVector &vals) {
      m_stringValues[key] = vals;
      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    StatusCode GenericEvent::getValues(const std::string &key, IntVector &vals) const {
      auto findIter = m_intValues.find(key);

      if (m_intValues.cend() != findIter) {
        vals.insert(vals.begin(), findIter->second.cbegin(), findIter->second.cend());
        return STATUS_CODE_SUCCESS;
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    StatusCode GenericEvent::getValues(const std::string &key, FloatVector &vals) const {
      auto findIter = m_floatValues.find(key);

      if (m_floatValues.cend() != findIter) {
        vals.insert(vals.begin(), findIter->second.cbegin(), findIter->second.cend());
        return STATUS_CODE_SUCCESS;
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    StatusCode GenericEvent::getValues(const std::string &key, DoubleVector &vals) const {
      auto findIter = m_doubleValues.find(key);

      if (m_doubleValues.cend() != findIter) {
        vals.insert(vals.begin(), findIter->second.cbegin(), findIter->second.cend());
        return STATUS_CODE_SUCCESS;
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    StatusCode GenericEvent::getValues(const std::string &key, StringVector &vals) const {
      auto findIter = m_stringValues.find(key);

      if (m_stringValues.cend() != findIter) {
        vals.insert(vals.begin(), findIter->second.cbegin(), findIter->second.cend());
        return STATUS_CODE_SUCCESS;
      }

      return STATUS_CODE_NOT_FOUND;
    }

  }
}
