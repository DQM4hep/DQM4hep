//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#include "dqm4hep/Client.h"

using namespace dqm4hep::net;
using namespace dqm4hep::core;

class ServicePrinter {
public:
  ServicePrinter(const std::string &name, const std::string &printMethod)
      : m_serviceName(name), m_printMethod(printMethod) {
  }
  void print(const Buffer &buffer);
  template <typename T>
  void printT(const Buffer &buffer);

private:
  std::string m_serviceName;
  std::string m_printMethod;
};

template <typename T>
inline void ServicePrinter::printT(const Buffer &buffer) {
  T value;
  memcpy(&value, buffer.begin(), buffer.size());
  std::cout << m_serviceName << " : " << value << std::endl;
}

template <>
inline void ServicePrinter::printT<json>(const Buffer &buffer) {
  json value = json::parse(buffer.begin(), buffer.end());
  std::cout << "=== " << m_serviceName << " ===" << std::endl << value.dump(2) << std::endl;
}

inline void ServicePrinter::print(const Buffer &buffer) {
  const char *ptr = buffer.begin();
  size_t size = buffer.size();

  if (m_printMethod == "str") {
    std::string value(ptr, size);
    std::cout << value << std::endl;
  } else if (m_printMethod == "raw") {
    for (size_t i = 0; i < size; i++) {
      std::cout << std::hex << ptr[i];
      if (!i % 100)
        std::cout << std::endl;
    }
  } else if (m_printMethod == "float")
    this->printT<float>(buffer);
  else if (m_printMethod == "int")
    this->printT<int>(buffer);
  else if (m_printMethod == "uint")
    this->printT<unsigned int>(buffer);
  else if (m_printMethod == "double")
    this->printT<double>(buffer);
  else if (m_printMethod == "short")
    this->printT<short>(buffer);
  else if (m_printMethod == "long")
    this->printT<long>(buffer);
  else if (m_printMethod == "ulong")
    this->printT<unsigned long>(buffer);
  else if (m_printMethod == "ullong")
    this->printT<unsigned long long>(buffer);
  else if (m_printMethod == "json")
    this->printT<json>(buffer);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Usage : dqm4hep-subscribe-service name [raw|str|type]" << std::endl;
    return 1;
  }

  std::string name(argv[1]);
  std::string printMethod(argc >= 3 ? argv[2] : "raw");

  ServicePrinter printer(name, printMethod);
  Client client;
  client.subscribe(name, &printer, &ServicePrinter::print);

  while (1)
    sleep(1);

  return 0;
}
