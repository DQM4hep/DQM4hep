//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : T.Coates
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

    /** Chi2Test class
     */
    class Chi2Test : public QualityTest {
    public:
      class Factory : public QualityTestFactory {
      public:
        QTestPtr createQualityTest(const std::string &qname) const override;
      };

      Chi2Test(const std::string &name);
      ~Chi2Test() override = default;
      StatusCode readSettings(const dqm4hep::core::TiXmlHandle xmlHandle) override;
      std::string getTestOptions(std::string comparisonType);
      void userRun(MonitorElement* monitorElement, QualityTestReport &report) override;

    protected:
      std::string m_comparisonType;
      bool m_useUnderflow = {false};
      bool m_useOverflow = {false};

    };

    typedef Chi2Test::Factory Chi2TestFactory;

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline QTestPtr Chi2Test::Factory::createQualityTest(const std::string &qname) const {
      return std::shared_ptr<QTest>(new Chi2Test(qname));
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    Chi2Test::Chi2Test(const std::string &qname)
        : QualityTest("Chi2", qname),
	  m_comparisonType("UU")
{
      m_description = "Performs the chi-squared test on a TH1 and a reference, outputting the p-value. This is analogous to the Kolmogorov-Smirnov "
	              "test but is appropriate for use on binned data like histograms, which the Kolmogorov test is not.";
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode Chi2Test::readSettings(const TiXmlHandle xmlHandle) {
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "ComparisonType", m_comparisonType));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "UseUnderflow", m_useUnderflow));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "UseOverflow", m_useOverflow));

      return STATUS_CODE_SUCCESS;
    }

    std::string Chi2Test::getTestOptions(std::string comparisonType) {

      std::string optionsString;

      if (comparisonType == "UU") {
	optionsString = "UU";
      }
      else if (comparisonType == "UW") {
	optionsString = "UW";	
      }
      else if (comparisonType == "WW") {
	optionsString = "WW";
      }
      else if (comparisonType == "NORM") {
	optionsString = "NORM";
      }

      if (m_useUnderflow) {
	optionsString += "UF";
      }
      if (m_useOverflow) {
	optionsString += "OF";
      }

      return optionsString;
    }

    void Chi2Test::userRun(MonitorElement* pMonitorElement, QualityTestReport &report) {
      const bool hasObject = (pMonitorElement->object() != nullptr);
      const bool hasReference = (pMonitorElement->reference() != nullptr);

      if (!hasObject) {
        report.m_message = "No object attached to monitor element";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_INVALID_PTR);
      }

      if (!hasReference) {
        report.m_message = "No reference attached to monitor element";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_INVALID_PTR);
      }

      const bool isObjHistogram = pMonitorElement->object()->InheritsFrom("TH1");
      const bool isRefHistogram = pMonitorElement->reference()->InheritsFrom("TH1");

      if (!isObjHistogram or !isRefHistogram) {
        report.m_message = "Both the object and the reference must be a TH1";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }

      TH1* pHistogram = pMonitorElement->objectTo<TH1>();
      TH1* pReferenceHistogram = pMonitorElement->referenceTo<TH1>();
      std::string options = Chi2Test::getTestOptions(m_comparisonType);

      report.m_quality = pHistogram->Chi2Test(pReferenceHistogram, options.c_str());
    }
    
    DQM_PLUGIN_DECL(Chi2TestFactory, "Chi2Test");
  }
}
