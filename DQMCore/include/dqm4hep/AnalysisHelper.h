//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : T.Coates, R.Ete
//====================================================================

#ifndef DQM4HEP_ANALYSISHELPER_H
#define DQM4HEP_ANALYSISHELPER_H

// Need to include ROOT headers so that it can interpret things like TH1 etc.

// -- dqm4hep headers
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElement.h>

// -- root headers
#include <TH1.h>
#include <TMath.h>

namespace dqm4hep {

  namespace core {

    /** analysis helper class
     */
    class AnalysisHelper
    {
    public:

      static float mean(MonitorElement* pMonitorElement, float percentage = 1.0);

      static float mean90(MonitorElement* pMonitorElement);

      static float rms(MonitorElement* pMonitorElement, float percentage = 1.0);

      static float rms90(MonitorElement* pMonitorElement);

      static float median(MonitorElement* pMonitorElement);

    };

  }

}

#endif // DQM4HEP_ANALYSISHELPER_H
