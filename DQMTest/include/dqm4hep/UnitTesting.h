//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_UNITTESTING_H
#define DQM4HEP_UNITTESTING_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>

namespace dqm4hep {

  namespace test {

    class UnitTest {
    public:
      UnitTest(const std::string &name, bool abortOnFail = true) :
      m_name(name),
      m_abortOnFail(abortOnFail) {
        core::Logger::createLogger(name, {core::Logger::coloredConsole()});
        core::Logger::setMainLogger(name);
        core::Logger::setLogLevel(spdlog::level::debug);
        dqm_info("==================================");
        dqm_info("== Starting unit test '{0}'", m_name);
        dqm_info("==================================");
      }
      
      ~UnitTest() {
        printSummary();
      }
      
      template <typename U, typename V>
      void test(const std::string& test, const U& lhs, const V& rhs) {
        std::stringstream sstr;
        sstr << "[TEST:" << test << "] ";  
        if( lhs == rhs ) {
          sstr << "'" << lhs << "' == '" << rhs << "'";
          pass( sstr.str() );
        }
        else {
          sstr << "'" << lhs << "' != '" << rhs << "'";
          error( sstr.str() );
        }
      }
      
      template <typename C>
      void test(const std::string& test, const C& condition) {
        std::stringstream sstr;
        sstr << "[TEST:" << test << "] ";  
        if( (condition) ) {
          sstr << "'" << condition << "'";
          pass( sstr.str() );
        }
        else {
          sstr << "'" << condition << "'";
          error( sstr.str() );
        }
      }
      
      void error(const std::string& message) {
        m_failed++;
        dqm_error( message );
        if( m_abortOnFail ) {
          dqm_error("Abort on fail !");
          printSummary();
          exit(1);
        }
      }
      
      void pass(const std::string& message) {
        m_passed++;
        dqm_info( message );
      }
      
      std::string status() {
        return ( m_failed > 0 ? "TEST_FAILED" : "TEST_PASSED");
      }
      
      void printSummary() {
        dqm_info("==================================");
        dqm_info("[{0}]: {1} PASSED, {2} FAILED", m_name, m_passed, m_failed);
        dqm_info( status() );
        dqm_info("==================================");
      }
      
    private:
      std::string          m_name = {""};
      bool                 m_abortOnFail = {true};
      unsigned int         m_passed = {0};
      unsigned int         m_failed = {0};
    };

  }
  
}

#endif //  DQM4HEP_UNITTESTING_H
