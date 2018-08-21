//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_JSON_H
#define DQM4HEP_JSON_H

#include <nlohmann/json.h>

namespace dqm4hep {

  namespace core {

    using json = nlohmann::json;
  }
}

#endif //  DQM4HEP_JSON_H