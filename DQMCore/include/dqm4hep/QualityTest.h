//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_QUALITYTEST_H
#define DQM4HEP_QUALITYTEST_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/json.h>

// -- root headers
#include <TObject.h>

namespace dqm4hep {

  namespace core {

    class TiXmlHandle;
    
    enum QualityFlag {
      UNDEFINED,
      INVALID,
      INSUFFICENT_STAT,
      SUCCESS,
      WARNING,
      ERROR
    };

    /** QualityTestReport class
     *  Hanlde the result of a quality test
     */
    class QualityTestReport {
    public:
      /** Constructor
       */
      QualityTestReport() = default;

      /** Copy constructor
       */
      QualityTestReport(const QualityTestReport &qreport);

      /** Assignment operator
       */
      QualityTestReport &operator=(const QualityTestReport &qreport);

      /**
       * [toJson description]
       * @param value [description]
       */
      void toJson(json &value) const;

      /**
       * [fromJson description]
       * @param value [description]
       */
      void fromJson(const json &value);

    public:
      std::string m_qualityTestName = {""};
      std::string m_qualityTestType = {""};
      std::string m_qualityTestDescription = {""};
      std::string m_monitorElementName = {""};
      std::string m_monitorElementType = {""};
      std::string m_monitorElementPath = {""};
      std::string m_message = {""};
      QualityFlag m_qualityFlag = {UNDEFINED};
      float m_quality = {0.f};
      json m_extraInfos = {};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** QReportStorage class
     */
    class QReportStorage {
    public:
      /** Constructor
       */
      QReportStorage();

      /** Add report to list. If report already exists, replace it.
       *  Comparison made from :
       *    - monitor element name
       *    - monitor element type
       *    - quality test name
       */
      void addReport(const QReport &report, bool warnOnReplace = true);

      /** Add reports to list. If reports already exists, replace it.
       *  Comparison made from :
       *    - monitor element name
       *    - monitor element type
       *    - quality test name
       */
      void addReports(const QReportMap &reports, bool warnOnReplace = true);

      /** Get a specific report
       */
      StatusCode report(const std::string &path, const std::string &name, const std::string &qualityTestName,
                        QReport &report);

      /** Get all reports of the monitor element
       */
      StatusCode reports(const std::string &path, const std::string &name, QReportMap &reports);

      /** Get all reports of the monitor element with quality higher than a limit (range [0,1])
       */
      StatusCode reportsQualityHigher(const std::string &path, const std::string &name, float qlimit,
                                      QReportMap &reports);

      /** Get all reports of the monitor element with quality lower than a limit (range [0,1])
       */
      StatusCode reportsQualityLower(const std::string &path, const std::string &name, float qlimit,
                                     QReportMap &reports);

      /** Get all reports
       */
      const QReportContainer &reports();

      /** Get all reports with quality higher than a limit (range [0,1])
       */
      StatusCode reportsQualityHigher(float qlimit, QReportContainer &reports);

      /** Get all reports with quality lower than a limit (range [0,1])
       */
      StatusCode reportsQualityLower(float qlimit, QReportContainer &reports);

      /** Clear all contents
       */
      void clear();
      
      /**
       *  @brief  Convert all stored reports to json format
       *  
       *  @param  object the json object to receive
       */
      void toJson(json &object) const;

    private:
      QReportContainer m_reports;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** QualityTest class
     */
    class QualityTest {
    public:
      /** Constructor
       */
      QualityTest(const std::string &type, const std::string &name);

      /** Destructor
       */
      virtual ~QualityTest();

      /** Get the quality test type
       */
      const std::string &type() const;

      /** Get the quality test name (unique identifier)
       */
      const std::string &name() const;

      /** Get the quality test description
       */
      const std::string &description() const;

      /** Perform the quality test result and fill the quality test report
       */
      void run(MonitorElement* monitorElement, QualityTestReport &report);

      /** Read the settings from the xml handle
       */
      virtual StatusCode readSettings(const dqm4hep::core::TiXmlHandle xmlHandle) = 0;

      /** Initialize the quality test.
       */
      virtual StatusCode init();
      
      /**
       *  @brief  Whether the monitor element has enough statistics to perform the qtest
       *
       *  @param  monitorElement the monitor element to check
       */
      virtual bool enoughStatistics(MonitorElement* monitorElement) const;

      /**
       *  @brief  Set the warning and error limits on quality test result.
       *          The limits must be ordered as 0 < error < warning < 1, else throws an exception
       *
       *  @param  warning the warning limit
       *  @param  error the error limit
       */
      void setLimits(float warning, float error);

      /**
       *  @brief  Get the warning limit
       */
      float warningLimit() const;

      /**
       *  @brief  Get the error limit
       */
      float errorLimit() const;
      
      /**
       *  @brief  Get the default warning limit  
       */
      static float defaultWarningLimit();
      
      /**
       *  @brief  Get the default error limit  
       */
      static float defaultErrorLimit();

    protected:
      /** Runs a quality test on the given monitor element and return a quality estimate
       */
      virtual void userRun(MonitorElement* monitorElement, QualityTestReport &report) = 0;

      /** Fill basic info in the qtest report.
       *  Must be called at start of qtest run
       */
      void fillBasicInfo(MonitorElement* monitorElement, QualityTestReport &report) const;

    private:
      std::string          m_type = {""};  ///< Quality test type (usually class name)
      std::string          m_name = {""};  ///< Quality test name
      float                m_warningLimit = {m_defaultWarningLimit}; ///< The quality test warning threshold
      float                m_errorLimit = {m_defaultErrorLimit};   ///< The quality test error threshold

    protected:
      std::string          m_description = {""}; ///< Quality test description
      static float         m_defaultWarningLimit;
      static float         m_defaultErrorLimit;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** QualityTestFactory class
     */
    class QualityTestFactory {
    public:
      /** Destructor
       */
      virtual ~QualityTestFactory();

      /** Create a quality test
       */
      virtual QTestPtr createQualityTest(const std::string &name) const = 0;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline StatusCode QualityTest::init() {
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    inline bool QualityTest::enoughStatistics(MonitorElement * /*monitorElement*/) const {
      return true;
    }

    //-------------------------------------------------------------------------------------------------

    inline QualityTestFactory::~QualityTestFactory() {
      /* nop */
    }
  }
}

#endif //  DQM4HEP_QUALITYTEST_H
