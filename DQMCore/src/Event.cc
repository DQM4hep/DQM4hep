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

// -- root headers
#include <TBuffer.h>

namespace dqm4hep {

  namespace core {

    void Event::clear() {
      m_type = UNKNOWN_EVENT;
      m_source = "unknwon";
      m_timeStamp = core::time::point();
      m_eventSize = 0;
      m_eventNumber = 0;
      m_runNumber = 0;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void Event::setStreamerName(const std::string &name) {
      m_streamerName = name;
    }
    
    //-------------------------------------------------------------------------------------------------

    const std::string &Event::getStreamerName() const {
      return m_streamerName;
    }

    //-------------------------------------------------------------------------------------------------

    void Event::writeBase(TBuffer &buffer) const {
      Int_t type(static_cast<Int_t>(m_type));
      int32_t timeStamp = core::time::asTime(m_timeStamp);
      buffer.WriteInt(type);
      buffer.WriteStdString(&m_source);
      buffer.WriteInt(timeStamp);
      buffer.WriteLong64(m_eventSize);
      buffer.WriteInt(m_eventNumber);
      buffer.WriteInt(m_runNumber);
    }

    //-------------------------------------------------------------------------------------------------

    void Event::readBase(TBuffer &buffer) {
      Int_t type(0);
      int32_t timeStamp(0);
      Long64_t eventSize(0);
      Int_t eventNumber(0);
      Int_t runNumber(0);
      buffer.ReadInt(type);
      buffer.ReadStdString(&m_source);
      buffer.ReadInt(timeStamp);
      buffer.ReadLong64(eventSize);
      buffer.ReadInt(eventNumber);
      buffer.ReadInt(runNumber);
      setType(static_cast<EventType>(type));
      setTimeStamp(core::time::asPoint(timeStamp));
      m_eventSize = eventSize;
      m_eventNumber = eventNumber;
      m_runNumber = runNumber;
    }
  }
}
