//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_STATUS_CODES_H
#define DQM4HEP_STATUS_CODES_H

// -- std headers
#include <exception>
#include <string>
#if defined(__GNUC__) && defined(BACKTRACE)
#include <cstdlib>
#include <execinfo.h>
#endif

#define RETURN_RESULT_IF(StatusCode1, Operator, Command)                                                               \
  {                                                                                                                    \
    const dqm4hep::core::StatusCode statusCode(Command);                                                               \
    if (statusCode Operator StatusCode1) {                                                                             \
      DQM4HEP_NO_EXCEPTION( dqm_error("{0} return {1}, ", #Command, dqm4hep::core::statusCodeToString(statusCode)); ); \
      dqm_error("    in function: {0}", __FUNCTION__);                                                                 \
      dqm_error("    in file:     {0} line#: {1}", __FILE__, __LINE__);                                                \
      return statusCode;                                                                                               \
    }                                                                                                                  \
  }

//-------------------------------------------------------------------------------------------------

#define RETURN_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)                                           \
  {                                                                                                                    \
    const dqm4hep::core::StatusCode statusCode(Command);                                                               \
    if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2)) {                                      \
      DQM4HEP_NO_EXCEPTION( dqm_error("{0} return {1}, ", #Command, dqm4hep::core::statusCodeToString(statusCode)); ); \
      dqm_error("    in function: {0}", __FUNCTION__);                                                                 \
      dqm_error("    in file:     {0} line#: {1}", __FILE__, __LINE__);                                                \
      return statusCode;                                                                                               \
    }                                                                                                                  \
  }

//-------------------------------------------------------------------------------------------------

#define THROW_RESULT_IF(StatusCode1, Operator, Command)                                                                \
  {                                                                                                                    \
    const dqm4hep::core::StatusCode statusCode(Command);                                                               \
    if (statusCode Operator StatusCode1) {                                                                             \
      DQM4HEP_NO_EXCEPTION( dqm_error("{0} return {1}, ", #Command, dqm4hep::core::statusCodeToString(statusCode)); ); \
      dqm_error("    in function: {0}", __FUNCTION__);                                                                 \
      dqm_error("    in file:     {0} line#: {1}", __FILE__, __LINE__);                                                \
      throw dqm4hep::core::StatusCodeException(statusCode);                                                            \
    }                                                                                                                  \
  }

//-------------------------------------------------------------------------------------------------

#define THROW_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)                                            \
  {                                                                                                                    \
    const dqm4hep::core::StatusCode statusCode(Command);                                                               \
    if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2)) {                                      \
      DQM4HEP_NO_EXCEPTION( dqm_error("{0} return {1}, ", #Command, dqm4hep::core::statusCodeToString(statusCode)); ); \
      dqm_error("    in function: {0}", __FUNCTION__);                                                                 \
      dqm_error("    in file:     {0} line#: {1}", __FILE__, __LINE__);                                                \
      throw dqm4hep::core::StatusCodeException(statusCode);                                                            \
    }                                                                                                                  \
  }

//-------------------------------------------------------------------------------------------------

#define PROCESS_CODE_IF_AND_RETURN(StatusCode1, Operator, Command, Code)                                               \
  {                                                                                                                    \
    const dqm4hep::core::StatusCode statusCode(Command);                                                               \
    if (statusCode Operator StatusCode1) {                                                                             \
      Code return statusCode;                                                                                          \
    }                                                                                                                  \
  }

//-------------------------------------------------------------------------------------------------

#define PROCESS_CODE_IF_AND_IF_AND_RETURN(StatusCode1, StatusCode2, Operator, Command, Code)                           \
  {                                                                                                                    \
    const dqm4hep::core::StatusCode statusCode(Command);                                                               \
    if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2)) {                                      \
      Code return statusCode;                                                                                          \
    }                                                                                                                  \
  }

//-------------------------------------------------------------------------------------------------

// definitions of the status code table
#define STATUS_CODE_TABLE(d) \
  d(STATUS_CODE_SUCCESS, "STATUS_CODE_SUCCESS") \
  d(STATUS_CODE_FAILURE, "STATUS_CODE_FAILURE") \
  d(STATUS_CODE_NOT_FOUND, "STATUS_CODE_NOT_FOUND") \
  d(STATUS_CODE_NOT_INITIALIZED, "STATUS_CODE_NOT_INITIALIZED") \
  d(STATUS_CODE_ALREADY_INITIALIZED, "STATUS_CODE_ALREADY_INITIALIZED") \
  d(STATUS_CODE_ALREADY_PRESENT, "STATUS_CODE_ALREADY_PRESENT") \
  d(STATUS_CODE_OUT_OF_RANGE, "STATUS_CODE_OUT_OF_RANGE") \
  d(STATUS_CODE_NOT_ALLOWED, "STATUS_CODE_NOT_ALLOWED") \
  d(STATUS_CODE_INVALID_PARAMETER, "STATUS_CODE_INVALID_PARAMETER") \
  d(STATUS_CODE_UNCHANGED, "STATUS_CODE_UNCHANGED") \
  d(STATUS_CODE_INVALID_PTR, "STATUS_CODE_INVALID_PTR")

// macros for enumerators
#define GET_ENUM_ENTRY(a, b) a,
#define GET_NAME_SWITCH(a, b)                                                                                          \
  case a:                                                                                                              \
    return b;

//-------------------------------------------------------------------------------------------------

namespace dqm4hep {

  namespace core {

    /** StatusCode enumerator
     */
    enum StatusCode { 
      STATUS_CODE_TABLE(GET_ENUM_ENTRY) 
      NUMBER_OF_STATUS_CODES 
    };

    std::string statusCodeToString(const StatusCode statusCode);

    //-------------------------------------------------------------------------------------------------

    /** StatusCodeException class
     */
    class StatusCodeException : public std::exception {
    public:
      /** Constructor
       */
      StatusCodeException(const StatusCode statusCode);
      StatusCodeException(const StatusCodeException &e) = default;

      /** Constructor
       */
      ~StatusCodeException() override;

      /** Get status code
       */
      StatusCode getStatusCode() const;

      /** Get status code as a string
       */
      std::string toString() const;

      /** Get back trace at point of exception construction (gcc only)
       */
      const std::string &getBackTrace() const;

    private:
      const StatusCode m_statusCode = {STATUS_CODE_SUCCESS};     ///< The status code
      std::string m_backTrace = {""};                            ///< The back trace
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline StatusCodeException::StatusCodeException(const StatusCode statusCode) : 
      m_statusCode(statusCode) {
#if defined(__GNUC__) && defined(BACKTRACE)
      const size_t maxDepth = 100;
      void *stackAddresses[maxDepth];

      size_t stackDepth = backtrace(stackAddresses, maxDepth);
      char **stackStrings = backtrace_symbols(stackAddresses, stackDepth);

      m_backTrace = "\nBackTrace\n    ";

      for (size_t i = 1; i < stackDepth; ++i) {
        m_backTrace += stackStrings[i];
        m_backTrace += "\n    ";
      }

      free(stackStrings); // malloc()ed by backtrace_symbols
#endif
    }

    //-------------------------------------------------------------------------------------------------

    inline StatusCodeException::~StatusCodeException() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    inline StatusCode StatusCodeException::getStatusCode() const {
      return m_statusCode;
    }

    //-------------------------------------------------------------------------------------------------

    inline std::string StatusCodeException::toString() const {
      return statusCodeToString(m_statusCode);
    }

    //-------------------------------------------------------------------------------------------------

    inline const std::string &StatusCodeException::getBackTrace() const {
      return m_backTrace;
    }

    //-------------------------------------------------------------------------------------------------

    inline std::string statusCodeToString(const StatusCode statusCode) {
      switch (statusCode) {
        STATUS_CODE_TABLE(GET_NAME_SWITCH)
      default:
        throw dqm4hep::core::StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }
    }
  }
}

#endif //  DQM4HEP_STATUS_CODES_H
