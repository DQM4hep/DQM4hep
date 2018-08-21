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

    /** KolmogorovTest class
     */
    class KolmogorovTest : public QualityTest {
    public:
      class Factory : public QualityTestFactory {
      public:
        QTestPtr createQualityTest(const std::string &qname) const override;
      };

      KolmogorovTest(const std::string &name);
      ~KolmogorovTest() override = default;
      StatusCode readSettings(const dqm4hep::core::TiXmlHandle xmlHandle) override;
      std::string getTestOptions(const bool isHistogram);
      void userRun(MonitorElement* monitorElement, QualityTestReport &report) override;

    protected:
      bool m_useUnderflow = {false};
      bool m_useOverflow = {false};

    };

    typedef KolmogorovTest::Factory KolmogorovTestFactory;

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline QTestPtr KolmogorovTest::Factory::createQualityTest(const std::string &qname) const {
      return std::shared_ptr<QTest>(new KolmogorovTest(qname));
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    KolmogorovTest::KolmogorovTest(const std::string &qname)
        : QualityTest("Kolmogorov", qname)
{
      m_description = "Performs the Kolmogorov-Smirnov test on a monitor element and a reference, outputting the p-value. In general this should only "
	              "be used for TGraphs. While this test can take TH1s, the Kolmogorov test is intended for use on unbinned data, not histograms. "
	              "See https://root.cern.ch/doc/master/classTH1.html#aeadcf087afe6ba203bcde124cfabbee4 for more information.";
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode KolmogorovTest::readSettings(const TiXmlHandle xmlHandle) {
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "UseUnderflow", m_useUnderflow));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "UseOverflow", m_useOverflow));

      return STATUS_CODE_SUCCESS;
    }

    std::string KolmogorovTest::getTestOptions(const bool isHistogram) {
      std::string optionsString = "";

      if (m_useUnderflow and isHistogram) {
	optionsString += "U";
      }
      if (m_useOverflow and isHistogram) {
	optionsString += "O";
      }
      return optionsString;
    }

    void KolmogorovTest::userRun(MonitorElement* pMonitorElement, QualityTestReport &report) {
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
      const bool isObjGraph = pMonitorElement->object()->InheritsFrom("TGraph");
      const bool isRefHistogram = pMonitorElement->reference()->InheritsFrom("TH1");
      const bool isRefGraph = pMonitorElement->reference()->InheritsFrom("TGraph");

      if ( (isObjGraph and !isRefGraph) or (isObjHistogram and !isRefHistogram) ) {
        report.m_message = "The object and reference must be of the same type";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }

      if (isObjGraph) {
	TGraph* pGraph = pMonitorElement->objectTo<TGraph>();
	TGraph* pReferenceGraph = pMonitorElement->referenceTo<TGraph>();
	std::string options = KolmogorovTest::getTestOptions(isObjHistogram);

	int sizeGraph = pGraph->GetN();
	int sizeRef = pReferenceGraph->GetN();

	if ( (sizeGraph == 0) or (sizeRef == 0) ) {
	  report.m_message = "One or more of the graphs are empty!";
	  report.m_quality = 0.0;
	  throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
	}

	pGraph->Sort();
	pReferenceGraph->Sort();
	double* pArrayGraph = pGraph->GetY();
	double* pArrayRef = pReferenceGraph->GetY();

	report.m_extraInfos["options"] = options;
	report.m_quality = TMath::KolmogorovTest(sizeGraph, pArrayGraph, sizeRef, pArrayRef, options.c_str());
      }
      else if (isObjHistogram) {
	TH1* pHistogram = pMonitorElement->objectTo<TH1>();
	TH1* pReferenceHistogram = pMonitorElement->referenceTo<TH1>();
	std::string options = KolmogorovTest::getTestOptions(isObjHistogram);

	report.m_extraInfos["options"] = options;
	report.m_message = "The Kolmogorov test isn't intended for histograms! Use caution when interepreting the quality.";
	report.m_quality = pHistogram->KolmogorovTest(pReferenceHistogram, options.c_str());
      }
      else {
	report.m_message = "ROOT monitor element object is of unrecognised type!";
        report.m_quality = 0.f;
	throw StatusCodeException(STATUS_CODE_INVALID_PTR);
      }

    }
    
    DQM_PLUGIN_DECL(KolmogorovTestFactory, "KolmogorovTest");
  }
}
