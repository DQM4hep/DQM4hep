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
#include <dqm4hep/Event.h>
#include <dqm4hep/EventReader.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/GenericEvent.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/XmlHelper.h>

namespace dqm4hep {

  namespace core {

    /**
     *  @brief  GenericEventXMLReader class
     *          Read an XML and read GenericEvent events from it
     */
    class GenericEventXMLReader : public EventReader {
    public:
      GenericEventXMLReader() = default;
      ~GenericEventXMLReader() override;
      GenericEventXMLReader(const GenericEventXMLReader&) = delete;
      GenericEventXMLReader& operator=(const GenericEventXMLReader&) = delete;

      core::StatusCode open(const std::string &fname) override;
      core::StatusCode skipNEvents(int nEvents) override;
      core::StatusCode runInfo(core::Run &run) override;
      core::StatusCode readNextEvent() override;
      core::StatusCode close() override;
      
    private:
      TiXmlDocument        m_document = {};
      TiXmlElement        *m_currentEvent = {nullptr};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    GenericEventXMLReader::~GenericEventXMLReader() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    core::StatusCode GenericEventXMLReader::open(const std::string &fname) {
      if(not m_document.LoadFile(fname)) {
        dqm_error("GenericEventXMLReader::open(): couldn't load xml file ['{0}', row: {1}, col: {2}] : {3}", fname,
                  m_document.ErrorRow(), m_document.ErrorCol(), m_document.ErrorDesc());
        return STATUS_CODE_FAILURE;
      }
      m_currentEvent = m_document.RootElement()->FirstChildElement("event");
      if(nullptr == m_currentEvent) {
        dqm_error( "No event stored in the file '{0}'", fname );
        return STATUS_CODE_FAILURE;
      }
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::StatusCode GenericEventXMLReader::skipNEvents(int nEvents) {
      int nSkippedEvents = 0;
      while(1) {
        if(nSkippedEvents == nEvents)
          break;
        m_currentEvent = m_currentEvent->NextSiblingElement("event");
        if(nullptr == m_currentEvent) {
          dqm_error( "Couldn't skip {0} events in file '{1}' : file has less than {1}", nEvents, m_document.ValueStr() );
          return STATUS_CODE_OUT_OF_RANGE;
        }
        nSkippedEvents++;
      }
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::StatusCode GenericEventXMLReader::runInfo(core::Run &run) {
      auto runInfoElement = m_document.RootElement()->FirstChildElement("run");
      if(!runInfoElement) {
        dqm_error( "No run info available in file '{0}'", m_document.ValueStr() );
        return STATUS_CODE_NOT_FOUND;
      }
      TiXmlHandle handle(runInfoElement);
      int runNumber = 0;
      int32_t startTime = 0, endTime = 0;
      std::string description, detectorName;
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "RunNumber", runNumber));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "StartTime", startTime));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "EndTime", endTime));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "Description", description));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "DetectorName", detectorName));
      auto parameters = runInfoElement->FirstChildElement("parameters");
      if(nullptr != parameters) {
        for(auto child = parameters->FirstChildElement("parameter") ; nullptr != child ; child = child->NextSiblingElement("parameter")) {
          std::string name, value;
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::parseParameterElement(child, name, value));
          run.setParameter(name, value);
        }
      }
      run.setRunNumber(runNumber);
      run.setStartTime(core::time::asPoint(startTime));
      run.setEndTime(core::time::asPoint(endTime));
      run.setDescription(description);
      run.setDetectorName(detectorName);
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::StatusCode GenericEventXMLReader::readNextEvent() {
      EventPtr event = GenericEvent::make_shared();
      GenericEvent *generic = event->getEvent<GenericEvent>();
      // end of file !
      if(nullptr == m_currentEvent) {
        return STATUS_CODE_OUT_OF_RANGE;
      }
      TiXmlHandle handle(m_currentEvent);
      int32_t eventType = static_cast<int32_t>(UNKNOWN_EVENT);
      std::string source;
      int32_t timeStamp = 0, eventNumber = 0, runNumber = 0;
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "EventType", eventType));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "Source", source));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "TimeStamp", timeStamp));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "EventNumber", eventNumber));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(handle, 
        "RunNumber", runNumber));
      event->setType(static_cast<EventType>(eventType));
      event->setSource(source);
      event->setTimeStamp(core::time::asPoint(timeStamp));
      event->setEventNumber(eventNumber);
      event->setRunNumber(runNumber);
      // loop over <field> elements
      for(auto field = m_currentEvent->FirstChildElement("field") ; nullptr != field ; field = field->NextSiblingElement("field")) {
        std::string name, type;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(field, "name", name));
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(field, "type", type));
        if("int" == type) {
          if(nullptr == field->GetText()) {
            return STATUS_CODE_FAILURE;
          }
          std::string content = field->GetText();
          IntVector values;
          core::tokenize(content, values, " ");
          generic->setValues(name, values);
        }
        else if("float" == type) {
          if(nullptr == field->GetText()) {
            return STATUS_CODE_FAILURE;
          }
          std::string content = field->GetText();
          FloatVector values;
          core::tokenize(content, values, " ");
          generic->setValues(name, values);
        }
        else if("double" == type) {
          if(nullptr == field->GetText()) {
            return STATUS_CODE_FAILURE;
          }
          std::string content = field->GetText();
          DoubleVector values;
          core::tokenize(content, values, " ");
          generic->setValues(name, values);
        }
        else if("string" == type) {
          StringVector values;
          // Wrap string into <str> </str> elements to avoid string tokenize
          for(auto strElement = field->FirstChildElement("str") ; nullptr != strElement ; strElement = strElement->NextSiblingElement("str")) {
            values.push_back(strElement->GetText() != nullptr ? strElement->GetText() : "");
          }
          generic->setValues(name, values);
        }
        else {
          dqm_warning( "Unrecognized field type '{0}' ! Skipping ...", type );
          continue;
        }
      }
      onEventRead().emit(event);
      m_currentEvent = m_currentEvent->NextSiblingElement("event");
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    core::StatusCode GenericEventXMLReader::close() {
      m_document.Clear();
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    DQM_PLUGIN_DECL(GenericEventXMLReader, "GenericEventXMLReader");
  }
}
