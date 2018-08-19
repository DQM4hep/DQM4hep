#ifdef __MAKECINT__

#pragma link off all class;
#pragma link off all function;
#pragma link off all global;
#pragma link off all typedef;

#pragma link C++ namespace dqm4hep;
#pragma link C++ namespace dqm4hep::core;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ class std::map<std::string, std::vector<int>>+;
#pragma link C++ class std::map<std::string, std::vector<double>>+;
#pragma link C++ class std::map<std::string, std::vector<float>>+;
#pragma link C++ class std::map<std::string, std::vector<std::string>>+;

#pragma link C++ class dqm4hep::core::TScalarInt + ;
#pragma link C++ class dqm4hep::core::TScalarReal + ;
#pragma link C++ class dqm4hep::core::TScalarFloat + ;
#pragma link C++ class dqm4hep::core::TScalarDouble + ;
#pragma link C++ class dqm4hep::core::TScalarString + ;
#pragma link C++ class dqm4hep::core::TScalarShort + ;
#pragma link C++ class dqm4hep::core::TScalarLong + ;
#pragma link C++ class dqm4hep::core::TScalarLong64_t + ;

#pragma link C++ class dqm4hep::core::TDynamicGraph + ;

#endif
