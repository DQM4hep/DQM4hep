//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_RUN_H
#define DQM4HEP_RUN_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/json.h>

// -- std headers
#include <ctime>
#include <string>

namespace dqm4hep {

  namespace core {

    /** 
     *  @brief  Run class
     */
    class Run {
    public:
      /** 
       *  @brief  Constructor
       *
       *  @param  runNumber the run number
       *  @param  description the run description
       *  @param  detectorName the detector name
       */
      Run(int runNumber = 0, const std::string &description = "", const std::string &detectorName = "");

      /** 
       *  @brief  Default destructor
       */
      ~Run() = default;

      /** 
       *  @brief  Get the run number
       */
      int runNumber() const;

      /** 
       *  @brief  Get the run start time
       */
      TimePoint startTime() const;

      /** 
       *  @brief  Get the run end time
       */
      TimePoint endTime() const;

      /** 
       *  @brief  Get the run description
       */
      const std::string &description() const;

      /** 
       *  @brief  Get the detector name
       */
      const std::string &detectorName() const;

      /** 
       *  @brief  Set the run number
       *
       *  @param  runNumber the run number
       */
      void setRunNumber(int runNumber);

      /** 
       *  @brief  Set the run start time
       *
       *  @param  startTime the run start time
       */
      void setStartTime(const TimePoint &startTime);

      /** 
       *  @brief  Set the run end tim
       *
       *  @param  endTime the run end time
       */
      void setEndTime(const TimePoint &endTime);

      /** 
       *  @brief  Set the run description
       *
       *  @param  description the run description
       */
      void setDescription(const std::string &description);

      /** 
       *  @brief  Set the detector name
       *
       *  @param  detectorName the detector name
       */
      void setDetectorName(const std::string &detectorName);

      /** 
       *  @brief  Set a run parameter
       *          The value is converted to string
       *
       *  @param  key the parameter name
       *  @param  value the parameter value to set
       */
      template <typename T>
      void setParameter(const std::string &key, const T &value);

      /** 
       *  @brief  Get a run parameter
       *          The value is converted into template type using the << operator
       *          If the parameter is not found, the value remains unchanged
       *
       *  @param  key the parameter name
       *  @param  value the value to receive
       */
      template <typename T>
      void parameter(const std::string &key, T &value) const;
      
      /** 
       *  @brief  Get a run parameter
       *          The value is converted into template type using the << operator
       *          If the parameter is not found, the fallback value is used instead
       *
       *  @param  key the parameter name
       *  @param  value the value to receive
       *  @param  fallback the fallback value
       */
      template <typename T>
      void parameter(const std::string &key, T &value, const T &fallback) const;

      /** 
       *  @brief  Get the number of run parameters
       */
      unsigned int nParameters() const;

      /** 
       *  @brief  Get the run parameters keys
       */
      StringVector parameterKeys() const;
      
      /**
       *  @brief  Get the run parameters
       */
      const StringMap &parameters() const;

      /** 
       *  @brief  Reset the run properties
       */
      void reset();

      /** 
       *  @brief  Convert the run object to json
       *
       *  @param  value the json value to receive 
       */
      void toJson(json &value) const;

      /** 
       *  @brief  Read and set run info from json object
       *
       *  @param  value the json object to read
       */
      void fromJson(const json &value);

    protected:
      /// The run number
      int                     m_runNumber = {0};
      /// The run start time
      TimePoint               m_startTime = {};
      /// The run end time
      TimePoint               m_endTime = {};
      /// The detector name
      std::string             m_detectorName = {""};
      /// The run description 
      std::string             m_description = {""};
      /// The run parameters
      StringMap               m_parametersMap = {};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    std::ostream& operator<<(std::ostream &out, const Run &run);

    //-------------------------------------------------------------------------------------------------

    inline int Run::runNumber() const {
      return m_runNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline TimePoint Run::startTime() const {
      return m_startTime;
    }

    //-------------------------------------------------------------------------------------------------

    inline TimePoint Run::endTime() const {
      return m_endTime;
    }

    //-------------------------------------------------------------------------------------------------

    inline const std::string &Run::description() const {
      return m_description;
    }

    //-------------------------------------------------------------------------------------------------

    inline const std::string &Run::detectorName() const {
      return m_detectorName;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setRunNumber(int rn) {
      m_runNumber = rn;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setStartTime(const TimePoint &st) {
      m_startTime = st;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setEndTime(const TimePoint &et) {
      m_endTime = et;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setDescription(const std::string &des) {
      m_description = des;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setDetectorName(const std::string &dn) {
      m_detectorName = dn;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Run::setParameter(const std::string &key, const T &value) {
      m_parametersMap[key] = dqm4hep::core::typeToString<T>(value);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Run::parameter(const std::string &key, T &value) const {
      auto findIter = m_parametersMap.find(key);
      if (m_parametersMap.end() == findIter)
        return;
      dqm4hep::core::stringToType(findIter->second, value);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline void Run::parameter(const std::string &key, T &value, const T &fallback) const {
      auto findIter = m_parametersMap.find(key);
      if (m_parametersMap.end() == findIter) {
        value = fallback;
        return;
      }
      dqm4hep::core::stringToType(findIter->second, value);
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void Run::setParameter(const std::string &key, const std::string &value) {
      m_parametersMap[key] = value;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void Run::parameter(const std::string &key, std::string &value) const {
      auto findIter = m_parametersMap.find(key);
      if (m_parametersMap.end() == findIter)
        return;
      value = findIter->second;
    }
    
    //-------------------------------------------------------------------------------------------------

    template <>
    inline void Run::parameter(const std::string &key, std::string &value, const std::string &fallback) const {
      auto findIter = m_parametersMap.find(key);
      if (m_parametersMap.end() == findIter) {
        value = fallback;
        return;
      }
      value = findIter->second;
    }

    //-------------------------------------------------------------------------------------------------

    inline unsigned int Run::nParameters() const {
      return m_parametersMap.size();
    }
  }
}

#endif //  DQM4HEP_RUN_H
