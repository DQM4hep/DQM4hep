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
#include "dqm4hep/Internal.h"
#include "dqm4hep/StatusCodes.h"
#include "dqm4hep/Cycle.h"
#include "dqm4hep/Module.h"
#include "dqm4hep/ModuleApi.h"
#include "dqm4hep/PluginManager.h"

namespace dqm4hep {

  namespace online {

    class TestAnalysisModule : public AnalysisModule {
    public:      
      TestAnalysisModule() = default;
      ~TestAnalysisModule() = default;
    private:
      OnlineElementPtr       m_testGraph = {nullptr};
      OnlineElementPtr       m_testGraph2D = {nullptr};
      OnlineElementPtr       m_testHisto1D = {nullptr};
      OnlineElementPtr       m_testHisto2D = {nullptr};
      OnlineElementPtr       m_testScalarInt = {nullptr};
      OnlineElementPtr       m_testScalarFloat = {nullptr};
      OnlineElementPtr       m_testScalarShort = {nullptr};
      OnlineElementPtr       m_testScalarString = {nullptr};
    private:
      void readSettings(const core::TiXmlHandle &handle);
      void initModule();
      void startOfRun(core::Run &run);
      void startOfCycle();
      void endOfCycle(const EOCCondition &condition);
      void endOfRun(const core::Run &run);
      void endModule();
      void process(core::EventPtr event);
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    void TestAnalysisModule::readSettings(const core::TiXmlHandle &handle) {
      auto element = handle.Element();
      core::TiXmlPrinter printer;
      element->Accept(&printer);
      dqm_info( "Received the following XML settings:" );
      dqm_info( printer.Str() );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestAnalysisModule::initModule() {
      dqm_info( "TestAnalysisModule::initModule" );
      // Create graphs in the Graphs directory
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::mkdir(this, "Graphs"));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::cd(this, "Graphs"));
      
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookObject<TGraph>(this, 
        m_testGraph, ".", "MyGraph", "The graph title"));
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookObject<TGraph2D>(this, 
        m_testGraph2D, ".", "MyGraph2D", "The graph 2D title"));
        
      // Create the histograms in the Histos directory
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::cd(this));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::mkdir(this, "Histos"));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::cd(this, "Histos"));
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookHisto<TH1D>(this, 
        m_testHisto1D, ".", "MyHisto1D", "The histo 1D title", 10, 0, 45));
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookHisto<TH2D>(this, 
        m_testHisto2D, ".", "MyHisto2D", "The histo 2D title", 100, 0., 99., 90, 0, 89));
        
      // Create the scalars in the Scalars directory
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::cd(this));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::mkdir(this, "Scalars"));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::cd(this, "Scalars"));
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookScalar<int>(this, 
        m_testScalarInt, ".", "MyScalarInt", "The scalar int title", 42));
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookScalar<float>(this, 
        m_testScalarFloat, ".", "MyScalarFloat", "The scalar float title", 785.f));
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookScalar<short>(this, 
        m_testScalarShort, ".", "MyScalarShort", "The scalar short title", 11));
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::bookScalar<std::string>(this, 
        m_testScalarString, ".", "MyScalarString", "The scalar string title", "Pimp my DQM !"));
        
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::cd(this));
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, ModuleApi::dump(this));
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestAnalysisModule::startOfRun(core::Run &run) {
      dqm_info( "=> TestAnalysisModule::startOfRun: {0}", core::typeToString(run) );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestAnalysisModule::startOfCycle() {
      dqm_info( "====> TestAnalysisModule::startOfCycle" );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestAnalysisModule::endOfCycle(const EOCCondition &condition) {
      dqm_info( "====> TestAnalysisModule::endOfCycle: {0}", core::typeToString(condition) );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestAnalysisModule::endOfRun(const core::Run &run) {
      dqm_info( "=> TestAnalysisModule::endOfRun: {0}", core::typeToString(run) );
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void TestAnalysisModule::endModule() {
      dqm_info( "TestAnalysisModule::endModule");
    }
    
    //-------------------------------------------------------------------------------------------------

    void TestAnalysisModule::process(core::EventPtr event) {
      dqm_info( "========> TestAnalysisModule::process: received event:");
      dqm_info( "========== Event {0}, run {1} ==", event->getEventNumber(), event->getRunNumber());
      dqm_info( "============> Event size: {0} bytes", event->getEventSize());
      dqm_info( "============> Time stamp: {0} ", core::typeToString(std::chrono::system_clock::to_time_t(event->getTimeStamp())));
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    DQM_PLUGIN_DECL(TestAnalysisModule, "TestAnalysisModule");
  }

}
