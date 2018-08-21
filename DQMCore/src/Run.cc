//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include <dqm4hep/Run.h>

namespace dqm4hep {

  namespace core {

    Run::Run(int rn, const std::string &des, const std::string &dn) : 
      m_runNumber(rn),
      m_startTime(dqm4hep::core::time::now()),
      m_detectorName(dn),
      m_description(des) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    StringVector Run::parameterKeys() const {
      StringVector keys;
      for (auto iter = m_parametersMap.begin(), endIter = m_parametersMap.end(); endIter != iter; ++iter)
        keys.push_back(iter->first);
      return keys;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    const StringMap &Run::parameters() const {
      return m_parametersMap;
    }

    //-------------------------------------------------------------------------------------------------

    void Run::reset() {
      m_runNumber = 0;
      m_startTime = core::time::point();
      m_endTime = core::time::point();
      m_detectorName.clear();
      m_description.clear();
      m_parametersMap.clear();
    }

    //-------------------------------------------------------------------------------------------------

    void Run::toJson(json &value) const {
      auto st = core::time::asTime(m_startTime);
      auto et = core::time::asTime(m_endTime);
      value = {
        {"runNumber", m_runNumber},
        {"startTime", st},
        {"endTime", et},
        {"detector", m_detectorName},
        {"description", m_description}, 
        {"parameters", m_parametersMap}
      };
    }

    //-------------------------------------------------------------------------------------------------

    void Run::fromJson(const json &value) {
      m_runNumber = value.value<int>("runNumber", 0);
      auto st = value.value<int32_t>("startTime", 0);
      auto et = value.value<int32_t>("endTime", 0);
      m_startTime = core::time::asPoint(st);
      m_endTime = core::time::asPoint(et);
      m_detectorName = value.value<std::string>("detector", "");
      m_description = value.value<std::string>("description", "");
      m_parametersMap = value.value<StringMap>("parameters", StringMap());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::ostream& operator<<(std::ostream &out, const Run &run) {
      core::json jrun;
      run.toJson(jrun);
      out << jrun.dump(2);
      return out;
    }
  }
}
