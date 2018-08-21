//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : T.Coates, R.Ete
//====================================================================

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/QualityTest.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/XmlHelper.h>
#include <dqm4hep/AnalysisHelper.h>

// -- root headers
#include <TH1.h>
#include <TMath.h>

namespace dqm4hep {

  namespace core {

    /** PropertyWithinExpectedTest class
     */
    class PropertyWithinExpectedTest : public QualityTest {
    public:
      class Factory : public QualityTestFactory {
      public:
        QTestPtr createQualityTest(const std::string &qname) const override;
      };

      PropertyWithinExpectedTest(const std::string &name);
      ~PropertyWithinExpectedTest() override = default;
      StatusCode readSettings(const dqm4hep::core::TiXmlHandle xmlHandle) override;
      void userRun(MonitorElement* monitorElement, QualityTestReport &report) override;

    protected:
      float m_expectedValue;
      float m_deviationLower;
      float m_deviationUpper;
      float m_percentage;
      std::string m_property;
      std::string m_method;
      //std::vector<std::string> m_properties;

    };

    typedef PropertyWithinExpectedTest::Factory PropertyWithinExpectedTestFactory;

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline QTestPtr PropertyWithinExpectedTest::Factory::createQualityTest(const std::string &qname) const {
      return std::shared_ptr<QTest>(new PropertyWithinExpectedTest(qname));
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    PropertyWithinExpectedTest::PropertyWithinExpectedTest(const std::string &qname)
        : QualityTest("PropertyWithinExpected", qname),
          m_expectedValue(0.f),
	  m_deviationLower(0.f),
	  m_deviationUpper(0.f),
	  m_percentage(1.0),
	  m_property(),
	  m_method("WithinRange")
{
      m_description = "Test if a given property (Mean, Mean90, RMS, RMS90, Median) of a histogram or graph is contained within the expected user-defined range, "
	              "or above or below a user-defined threshold. For 'within range' tests the quality is defined as the probability to be close to the mean: 1 at "
	              "the mean, 0 infinitely far from the mean (using TMath::Prob(chi2,1)). For threshold tests the quality is 1 if it passes, 0 otherwise.";
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode PropertyWithinExpectedTest::readSettings(const TiXmlHandle xmlHandle) {

      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, "Property", m_property));

      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "Percentage", m_percentage));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "Method", m_method));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "ExpectedValue", m_expectedValue));
      
      if (m_method == "HigherThan") {
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, "DeviationLower", m_deviationLower));
      }
      if (m_method == "LowerThan") {
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, "DeviationUpper", m_deviationUpper));
      }
      if (m_method == "WithinRange") {
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, "DeviationUpper", m_deviationUpper));
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::readParameter(xmlHandle, "DeviationLower", m_deviationLower));
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    void PropertyWithinExpectedTest::userRun(MonitorElement* monitorElement, QualityTestReport &report) {

      if (nullptr == monitorElement->objectTo<TH1>() && nullptr == monitorElement->objectTo<TGraph>()) {
	report.m_message = "ROOT monitor element object is of unrecognised type!";
	throw StatusCodeException(STATUS_CODE_INVALID_PTR);
      }

      // For-loop code for doing multiple test types per qtest; use TBD
      /*
      for (std::vector<std::string>::iterator it = m_properties.begin(); it != m_properties.end(); it++
      {
        // Do something
      }
      */

      float result;

      if (m_property == "Mean")
	{
	  result = AnalysisHelper::mean(monitorElement);
	}
      else if (m_property == "Mean90")
	{
	  result = AnalysisHelper::mean90(monitorElement);
	}
      else if (m_property == "RMS")
	{
	  result = AnalysisHelper::rms(monitorElement);
	}
      else if (m_property == "RMS90")
	{
	  result = AnalysisHelper::rms90(monitorElement);
	}
      else if (m_property == "Median")
	{
	  result = AnalysisHelper::median(monitorElement);
	}
      else
	{
	  dqm_error("Error in qtest {0} : unrecognised test type {1}", name(), m_property);
	  throw StatusCodeException(STATUS_CODE_FAILURE);
	}

      if ( m_method == "LowerThan" ) {
	if (result < m_deviationUpper) {
	  report.m_message = 
	    "Below expected value: expected " + typeToString(m_deviationUpper) + ", got " + typeToString(result);
	  report.m_quality = 1.0;
	}
	else { 
	  report.m_message = 
	    "Above expected value: expected " + typeToString(m_deviationUpper) + ", got " + typeToString(result);
	  report.m_quality = 0.0;
	}
      }
      else if ( m_method == "HigherThan" ) {
	if (result > m_deviationLower) {
	  report.m_message = 
	    "Above expected value: expected " + typeToString(m_deviationLower) + ", got " + typeToString(result);
	  report.m_quality = 1.0;
	}
	else { 
	  report.m_message = 
	    "Below expected value: expected " + typeToString(m_deviationLower) + ", got " + typeToString(result);
	  report.m_quality = 0.0;
	}
      }
      else if ( m_method == "WithinRange" ) {
	const float range(fabs(m_deviationUpper - m_deviationLower));

	if (m_deviationLower < result && result < m_deviationUpper) {
	  report.m_message =
            "Within expected range: expected " + typeToString(m_expectedValue) + ", got " + typeToString(result);
	}
	else if (result < m_deviationLower || result > m_deviationUpper) {
	  report.m_message =
            "Out of expected range: expected " + typeToString(m_expectedValue) + ", got " + typeToString(result);
	}

	const float chi = (result - m_expectedValue) / range;
	report.m_quality = TMath::Prob(chi * chi, 1);

      }
      else {
	dqm_error("Error in qtest {0} : unrecognised test method {1}", name(), m_method);
	throw StatusCodeException(STATUS_CODE_FAILURE);
      }
    }

    DQM_PLUGIN_DECL(PropertyWithinExpectedTestFactory, "PropertyWithinExpectedTest");
  
  }
}
