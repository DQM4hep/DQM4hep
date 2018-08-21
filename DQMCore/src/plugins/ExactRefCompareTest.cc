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
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/PluginManager.h>
#include <dqm4hep/QualityTest.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/XmlHelper.h>

// -- root headers
#include <TH1.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TMath.h>

namespace dqm4hep {

  namespace core {

    /** 
     *  @brief  ExactRefCompareTest class
     *          Compare the monitor element to its reference.
     *          Look for an strict equality in terms of object content
     *          For TH1 case (and all derived):
     *          - same number of bins
     *          - same bin content, bin by bin
     *          - if underflow option is set, check underflow bin equality
     *          - if overflow option is set, check overflow bin equality
     *          For TGraph, TGraph2D cases:
     *          - check for all point equality (to floating point precision) 
     *          For any other TObject type:
     *          - use the method TObject::IsEqual() to assert equality
     */
    class ExactRefCompareTest : public QualityTest {
    public:
      class Factory : public QualityTestFactory {
      public:
        QTestPtr createQualityTest(const std::string &qname) const override;
      };

      ExactRefCompareTest(const std::string &qname);
      ~ExactRefCompareTest() override = default;
      StatusCode readSettings(const dqm4hep::core::TiXmlHandle xmlHandle) override;
      void userRun(MonitorElement* monitorElement, QualityTestReport &report) override;
      
    private:
      void doHistogramTest(MonitorElement* monitorElement, QualityTestReport &report);
      void doGraph1DTest(MonitorElement* monitorElement, QualityTestReport &report);
      void doGraph2DTest(MonitorElement* monitorElement, QualityTestReport &report);
      void doIsEqualTest(MonitorElement* monitorElement, QualityTestReport &report);
      
    private:
      bool m_compareUnderflow = {true};
      bool m_compareOverflow = {true};
    };

    typedef ExactRefCompareTest::Factory ExactRefCompareTestFactory;

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline QTestPtr ExactRefCompareTest::Factory::createQualityTest(const std::string &qname) const {
      return std::shared_ptr<QTest>(new ExactRefCompareTest(qname));
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    ExactRefCompareTest::ExactRefCompareTest(const std::string &qname)
        : QualityTest("ExactRefCompare", qname) {
      m_description = std::string("Compare the monitor element to its reference. ") + 
      "Look for an strict equality in terms of object content.\n" + 
      "For TH1 case (and all derived): \n" +
      "  - same number of bins\n" + 
      "  - same bin content, bin by bin" +
      "  - if underflow option is set, check underflow bin equality" +
      "  - if overflow option is set, check overflow bin equality" +
      "For TGraph, TGraph2D cases:" +
      "  - check for all point equality (to floating point precision)" +
      "For any other TObject type:" +
      "  - use the method TObject::IsEqual() to assert equality";
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode ExactRefCompareTest::readSettings(const TiXmlHandle xmlHandle) {

      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "CompareOverflow", m_compareOverflow));
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameter(xmlHandle, "CompareUnderflow", m_compareUnderflow));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    void ExactRefCompareTest::userRun(MonitorElement* monitorElement, QualityTestReport &report) {
      
      const bool hasObject = (monitorElement->object() != nullptr);
      const bool hasReference = (monitorElement->reference() != nullptr);

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
      
      const std::string objectClass(monitorElement->object()->ClassName());
      const std::string referenceClass(monitorElement->reference()->ClassName());
      const bool isHistogram = monitorElement->object()->InheritsFrom("TH1");
      const bool isGraph1D = monitorElement->object()->InheritsFrom("TGraph");
      const bool isGraph2D = monitorElement->object()->InheritsFrom("TGraph2D");
      const bool sameType = (objectClass == referenceClass);
      
      if(!sameType) {
        report.m_message = "Objet and reference do not have the same type";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      if (isHistogram) {
        this->doHistogramTest(monitorElement, report);
      }
      else if(isGraph1D) {
        this->doGraph1DTest(monitorElement, report);
      }
      else if(isGraph2D) {
        this->doGraph2DTest(monitorElement, report);
      }
      else {
        this->doIsEqualTest(monitorElement, report);
      }
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ExactRefCompareTest::doHistogramTest(MonitorElement* monitorElement, QualityTestReport &report) {
      
      TH1 *histogram = monitorElement->objectTo<TH1>();
      TH1 *reference = monitorElement->referenceTo<TH1>();
      int nBins(0), nBinsRef(0);
      
      if(histogram->GetDimension() == 1) {
        nBins = histogram->GetNbinsX();
        nBinsRef = reference->GetNbinsX();
      }
      else if(histogram->GetDimension() == 2) {
        nBins = histogram->GetNbinsX() * histogram->GetNbinsY();
        nBinsRef = reference->GetNbinsX() * reference->GetNbinsY();
      }
      else if(histogram->GetDimension() == 3) {
        nBins = histogram->GetNbinsX() * histogram->GetNbinsY() * histogram->GetNbinsZ();
        nBinsRef = reference->GetNbinsX() * reference->GetNbinsY() * reference->GetNbinsZ();
      }
      else {
        report.m_message = "Histogram with invalid dimension (dim = " + typeToString(histogram->GetDimension()) + ") !";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }
      
      if(nBins != nBinsRef) {
        report.m_message = "Histogram and reference do not have the same number of bins";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }
      
      const int firstBin = m_compareUnderflow ? 0 : 1;
      const int lastBin = m_compareOverflow ? nBins : nBins+1;
      unsigned int nDifferentBins(0);
      
      for(auto bin = firstBin ; bin <= lastBin ; ++bin) {
        
        const Double_t binContent(histogram->GetBinContent(bin));
        const Double_t binContentRef(reference->GetBinContent(bin));
        const bool binsAreDifferent(!(fabs(binContent-binContentRef) < std::numeric_limits<Double_t>::epsilon()));
        
        if(binsAreDifferent) {
          ++nDifferentBins;
        }
      }
      
      if(nDifferentBins != 0) {
        report.m_message = "Histogram and reference are different (difference = " + typeToString(nDifferentBins) + ")";
        // Quality estimate as the percentage of different points. 
        // This factor is scaled to the error limit to make sure the quality flag is set to ERROR
        report.m_quality = this->errorLimit() * (static_cast<float>(nBins) - static_cast<float>(nDifferentBins)) / static_cast<float>(nBins);
      }
      else {
        report.m_message = "Histogram and reference are equal !";
        report.m_quality = 1.f;
      }
      
      report.m_extraInfos["nDifferences"] = nDifferentBins;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ExactRefCompareTest::doGraph1DTest(MonitorElement* monitorElement, QualityTestReport &report) {
      
      TGraph *graph = monitorElement->objectTo<TGraph>();
      TGraph *reference = monitorElement->referenceTo<TGraph>();
      const unsigned int nPoints(graph->GetN());
      const unsigned int nPointsRef(reference->GetN());
      
      if(nPoints != nPointsRef) {
        report.m_message = "Graph and reference do not have the same number of points";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }
      
      unsigned int nDifferentPoints(0);
      
      for(unsigned int point = 0 ; point < nPoints ; ++point) {
        Double_t x, y, xRef, yRef;
        graph->GetPoint(point, x, y);
        reference->GetPoint(point, xRef, yRef);
        
        const bool xAreDifferent(!(fabs(x-xRef) < std::numeric_limits<Double_t>::epsilon()));
        const bool yAreDifferent(!(fabs(y-yRef) < std::numeric_limits<Double_t>::epsilon()));
        
        if(xAreDifferent || yAreDifferent) {
          ++nDifferentPoints;
        }
      }
      
      if(nDifferentPoints != 0) {
        report.m_message = "Graph and reference are different (difference = " + typeToString(nDifferentPoints) + ")";
        report.m_quality = this->errorLimit() * (static_cast<float>(nPoints) - static_cast<float>(nDifferentPoints)) / static_cast<float>(nPoints);
      }
      else {
        report.m_message = "Graph and reference are equal !";
        report.m_quality = 1.f;
      }
      
      report.m_extraInfos["nDifferences"] = nDifferentPoints;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ExactRefCompareTest::doGraph2DTest(MonitorElement* monitorElement, QualityTestReport &report) {
      
      TGraph2D *graph = monitorElement->objectTo<TGraph2D>();
      TGraph2D *reference = monitorElement->referenceTo<TGraph2D>();
      const unsigned int nPoints(graph->GetN());
      const unsigned int nPointsRef(reference->GetN());
      
      if(nPoints != nPointsRef) {
        report.m_message = "Graph and reference do not have the same number of points";
        report.m_quality = 0.f;
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }
      
      unsigned int nDifferentPoints(0);
      
      for(unsigned int point = 0 ; point < nPoints ; ++point) {
        const Double_t x = graph->GetX()[point];
        const Double_t y = graph->GetY()[point];
        const Double_t z = graph->GetZ()[point];
        const Double_t xRef = reference->GetX()[point];
        const Double_t yRef = reference->GetY()[point];
        const Double_t zRef = reference->GetZ()[point];
        
        const bool xAreDifferent(!(fabs(x-xRef) < std::numeric_limits<Double_t>::epsilon()));
        const bool yAreDifferent(!(fabs(y-yRef) < std::numeric_limits<Double_t>::epsilon()));
        const bool zAreDifferent(!(fabs(z-zRef) < std::numeric_limits<Double_t>::epsilon()));
        
        if(xAreDifferent || yAreDifferent || zAreDifferent) {
          ++nDifferentPoints;
        }
      }
      
      if(nDifferentPoints != 0) {
        report.m_message = "Graph and reference are different (difference = " + typeToString(nDifferentPoints) + ")";
        report.m_quality = this->errorLimit() * (static_cast<float>(nPoints) - static_cast<float>(nDifferentPoints)) / static_cast<float>(nPoints);
      }
      else {
        report.m_message = "Graph and reference are equal !";
        report.m_quality = 1.f;
      }
      
      report.m_extraInfos["nDifferences"] = nDifferentPoints;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void ExactRefCompareTest::doIsEqualTest(MonitorElement* monitorElement, QualityTestReport &report) {
      
      TObject *object = monitorElement->object();
      TObject *reference = monitorElement->reference();
      
      const bool equal(object->IsEqual(reference));
      
      if(!equal) {
        report.m_message = "Object and reference are different (using TObject::IsEqual(ref)).";
        report.m_quality = 0.f; // FIXME: a bit hard no ? ...
      }
      else {
        report.m_message = "Object and reference are equal (using TObject::IsEqual(ref))";
        report.m_quality = 1.f;
      }
      
      report.m_extraInfos["nDifferences"] = 1;
    }

    DQM_PLUGIN_DECL(ExactRefCompareTestFactory, "ExactRefCompareTest");
  }
}
