#ifndef DQM4HEP_JSON_H
#define DQM4HEP_JSON_H

#include <nlohmann/json.h>

namespace dqm4hep {

  namespace core {

    using json = nlohmann::json;
    // using json_pointer = nlohmann::json_pointer;
    // using adl_serializer = nlohmann::adl_serializer;
  }
}

#endif //  DQM4HEP_JSON_H