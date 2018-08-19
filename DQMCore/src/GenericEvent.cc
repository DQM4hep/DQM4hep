/// \file GenericEvent.cc
/*
 *
 * GenericEvent.cc source template automatically generated by a class generator
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
