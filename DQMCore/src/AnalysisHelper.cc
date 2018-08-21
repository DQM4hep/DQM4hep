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
#include <dqm4hep/AnalysisHelper.h>
#include <typeinfo>
#include <TH2.h>

namespace dqm4hep {

  namespace core {

    float AnalysisHelper::mean(MonitorElement* pMonitorElement, float percentage)
    {
      TH1 *pHistogram = pMonitorElement->objectTo<TH1>();
      TGraph *pGraph = pMonitorElement->objectTo<TGraph>();
      float result = 0.0;

      if (nullptr == pHistogram && nullptr == pGraph) {
	dqm_error("The monitor element {0} of type {1} could not be recognised.", pMonitorElement->name(), pMonitorElement->type());
	throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      if(fabs(percentage - 1.f) < std::numeric_limits<float>::epsilon()) {
	if (nullptr != pHistogram) {
	  result = pHistogram->GetMean(1);
	}
	if (nullptr != pGraph) {
	  result = pGraph->GetMean(2);
	}
      }
      else {
	if (nullptr != pHistogram) {
	  TAxis *axis = pHistogram->GetXaxis();
	  int nbins = axis->GetNbins();
	  int imean = axis->FindBin(pHistogram->GetMean());
	  float entries = percentage*pHistogram->GetEntries();
	  float w = pHistogram->GetBinContent(imean);
	  float x = pHistogram->GetBinCenter(imean);
	  float sumw = w;
	  float sumwx = w*x;

	  for (int i=1;i<nbins;i++) {
	    if (i>0) {
	      w = pHistogram->GetBinContent(imean-i);
	      x = pHistogram->GetBinCenter(imean-i);
	      sumw += w;
	      sumwx += w*x;
	    }
	    if (i<= nbins) {
	      w = pHistogram->GetBinContent(imean+i);
	      x = pHistogram->GetBinCenter(imean+i);
	      sumw += w;
	      sumwx += w*x;
	    }
	    if (sumw > entries) break;
	  }
	  result = sumwx/sumw;
	}
	if (nullptr != pGraph) {
	  double* arrayY = pGraph->GetY();
	  int entries = pGraph->GetN();
	  int startNum = 0.5*(1.0-percentage)*entries;
	  int endNum = entries-startNum;
	  double sum = 0.0;

	  for (int i=startNum; i<endNum; i++) {
	    sum += arrayY[i];
	  }
	  result = sum/(percentage*entries);
	}
      }
      return result;
    }

    float AnalysisHelper::mean90(MonitorElement* pMonitorElement)
    {
      return AnalysisHelper::mean(pMonitorElement, 0.9);
    }

    float AnalysisHelper::rms(MonitorElement* pMonitorElement, float percentage)
    {
      TH1 *pHistogram = pMonitorElement->objectTo<TH1>();
      TGraph *pGraph = pMonitorElement->objectTo<TGraph>();
      float result = 0.0;

      if (nullptr == pHistogram && nullptr == pGraph) {
	dqm_error("The monitor element {0} of type {1} could not be recognised.", pMonitorElement->name(), pMonitorElement->type());
	throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      if(fabs(percentage - 1.f) < std::numeric_limits<float>::epsilon()) {
	if (nullptr != pHistogram) {
	  result = pHistogram->GetRMS(1);
	}
	if (nullptr != pGraph) {
	  result = pGraph->GetRMS(2);
	}
      }
      else {
	if (nullptr != pHistogram) {
	  TAxis *axis = pHistogram->GetXaxis();
	  int nbins = axis->GetNbins();
	  int imean = axis->FindBin(pHistogram->GetMean());
	  float entries = percentage*pHistogram->GetEntries();
	  float w = pHistogram->GetBinContent(imean);
	  float x = pHistogram->GetBinCenter(imean);
	  float mean = pHistogram->GetMean();

	  float sumw = w;
	  float sumwx = w*pow(x-mean,2);

	  for (int i=1;i<nbins;i++) {
	    if (i>0) {
	      w = pHistogram->GetBinContent(imean-i);
	      x = pHistogram->GetBinCenter(imean-i);
	      sumw += w;
	      sumwx += w*pow(x-mean,2);
	    }
	    if (i<= nbins) {
	      w = pHistogram->GetBinContent(imean+i);
	      x = pHistogram->GetBinCenter(imean+i);
	      sumw += w;
	      sumwx += w*pow(x-mean,2);
	    }
	    if (sumw > entries) break;
	  }
	  result = pow(sumwx/sumw,0.5);
	}
	if (nullptr != pGraph) {
	  double* arrayY = pGraph->GetY();
	  int entries = pGraph->GetN();
	  int startNum = 0.5*(1.0-percentage)*entries;
	  int endNum = entries-startNum;
	  double sum = 0.0;
	  float mean = pGraph->GetMean(1);

	  for (int i=startNum; i<endNum; i++) {
	    sum += pow(arrayY[i]-mean,2);
	  }
	  result = pow(sum/(percentage*entries),0.5);
	}
      }
      return result;
    }

    float AnalysisHelper::rms90(MonitorElement* pMonitorElement)
    {
      return AnalysisHelper::rms(pMonitorElement, 0.9);
    }

    float AnalysisHelper::median(MonitorElement* pMonitorElement)
    {
      TH1 *pHistogram = pMonitorElement->objectTo<TH1>();
      TGraph *pGraph = pMonitorElement->objectTo<TGraph>();
      float result = 0.0;

      if (nullptr == pHistogram && nullptr == pGraph) {
	dqm_error("The monitor element {0} of type {1} could not be recognised.", pMonitorElement->name(), pMonitorElement->type());
	throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      if (nullptr != pHistogram) {
	Double_t xq[1];
	Double_t yq[1];
	xq[0] = 0.5;
	pHistogram->GetQuantiles(1, yq, xq);
	result = yq[0];
      }
      else if (nullptr != pGraph) {

	if (pGraph->GetN() == 0) {
	  dqm_error("The monitor element {0} is empty!",pMonitorElement->name());
	  throw StatusCodeException(STATUS_CODE_FAILURE);
	}

	double* arrayY = pGraph->GetY();
	int size = pGraph->GetN()+1;
       
	if (size % 2 == 0) {
	  result = arrayY[size/2];
	}
	else {
	  result = (arrayY[size/2] + arrayY[(size/2)-1])/2;
	}
      }
      return result;
    }

  }

}
