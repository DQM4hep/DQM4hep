//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_DBINTERFACE_H
#define DQM4HEP_DBINTERFACE_H

// -- dqm4hep headers
#include <dqm4hep/Logging.h>
#include <dqm4hep/StatusCodes.h>

// -- mysql headers
#include <mysql.h>

namespace dqm4hep {

  namespace core {

    /** DBInterface class
     */
    class DBInterface {
    public:
      /**
       *  @brief  Default constructor
       */
      DBInterface() = default;
      DBInterface(const DBInterface&) = delete;
      DBInterface& operator=(const DBInterface&) = delete;

      /**
       *  @brief  Constructor. Connect to database
       *
       *  @param  host the host name
       *  @param  user the db user name
       *  @param  password the user password (if any)
       *  @param  database the database name to use
       */
      DBInterface(const std::string &host, const std::string &user, const std::string &password,
                  const std::string &database);

      /**
       *  @brief  Destructor
       */
      virtual ~DBInterface();

      /**
       *  @brief  Connect to the mysql server
       */
      StatusCode connect();

      /**
       *  @brief  Disconnect from the database
       */
      StatusCode disconnect();

      /**
       *  @brief  Connect to the database.
       *          Not possible if a connection is being handled.
       *          Use disconnect() first and then connect()
       *
       *  @param  host the host name
       *  @param  user the db user name
       *  @param  password the user password (if any)
       *  @param  database the database name to use
       */
      StatusCode connect(const std::string &host, const std::string &user, const std::string &password,
                         const std::string &database);

      /**
       *  @brief  Set host, user, password and database.
       *          Not possible if a connection is being handled.
       *          Use disconnect() first and then connect()
       *
       *  @param  host the host name
       *  @param  user the db user name
       *  @param  password the user password (if any)
       *  @param  database the database name to use
       */
      StatusCode set(const std::string &host, const std::string &user, const std::string &password,
                     const std::string &database);

      /**
       *  @brief  Get the host name
       */
      const std::string &getHost() const;

      /**
       *  @brief  Get the user name
       */
      const std::string &getUser() const;

      /**
       *  @brief  Get the password
       */
      const std::string &getPassword() const;

      /**
       *  @brief  Get the data base name
       */
      const std::string &getDataBase() const;

      /**
       *  @brief  Send query to database
       *          The result is not converted.
       *
       *  @param  query the mysql query
       *  @param  pResult the result to handle as raw pointer
       */
      StatusCode queryRaw(const std::string &query, void *&pResult);

      /**
       *  @brief  The result is streamed in type T using DQM4HEP::stringToType()
       *
       *  @param  query the mysql query
       *  @param  result the result to handle in the specified type
       */
      template <typename T>
      StatusCode query(const std::string &query, T &result);

      /**
       *  @brief  Send query to database.
       *          The result is streamed in a vector.
       *          Each value of the vector is streamed using DQM4HEP::stringToType()
       *
       *  @param  query the mysql query
       *  @param  result the result to handle as a vector of specified type
       */
      template <typename T>
      StatusCode queryVector(const std::string &query, std::vector<T> &result);

      /**
       *  @brief  Send query to database
       *          The result is passed in the handler function for user parsing
       *
       *  @param  query the mysql query
       *  @param  handler a function to handle the mysql query result (lambda or static function)
       */
      template <typename Handler>
      StatusCode queryAndHandle(const std::string &query, Handler handler);

      /**
       *  @brief  Execute the query (no result expected)
       *
       *  @param  query the mysql query
       */
      StatusCode execute(const std::string &query);

      /**
       *  @brief  Whether a connection is being handled
       */
      bool isConnected();

      /**
       *  @brief  Create a new parameter table
       *
       *  @param  table the table name
       *  @param  ifNotExists whether to check for table existence on creation
       *  @param  dropExistingTable whether to drop the existing table before creating
       */
      StatusCode createParameterTable(const std::string &table, bool ifNotExists = true,
                                      bool dropExistingTable = false);

      /**
       *  @brief  Empty a parameter table (uses the TRUNCATE statement)
       *
       *  @param  table the table name
       */
      StatusCode emptyParameterTable(const std::string &table);

      /**
       *  @brief  Set the parameters in the table.
       *          The parameter table must exist.
       *
       *  @param  table the parameter table name
       *  @param  parameterValueMap a map of parameter name -> value to upload
       */
      StatusCode setParameters(const std::string &table, const StringMap &parameterValueMap);

      /**
       *  @brief  Backup a parameter table under the specified name.
       *          A copy of the existing table is done.
       *          The parameter table must exist !
       *          The original table is left unchanged
       *
       * @param  table the parameter table name to backup
       * @param  backupTable the new parameter table name
       */
      StatusCode backupParameterTable(const std::string &table, const std::string &backupTable);

      /**
       *  @brief  Dump the parameter table in standard output
       *
       *  @param  table the parameter table name
       */
      StatusCode dumpParameterTable(const std::string &table);

      /**
       *  @brief  Extract all parameters from the specified table
       *
       *  @param  table the parameter table name
       *  @param  parameterValueMap the map of parameter name -> value to receive
       */
      StatusCode getTableParameters(const std::string &table, StringMap &parameterValueMap);

    private:
      MYSQL *m_pMySQL = {nullptr};
      std::string m_host = {""};
      std::string m_user = {""};
      std::string m_password = {""};
      std::string m_database = {""};
      bool m_isConnected = {false};
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode DBInterface::query(const std::string &uquery, T &result) {
      if (!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;

      if (mysql_query(m_pMySQL, uquery.c_str())) {
        dqm_error("MySQL query failed : {0}", mysql_error(m_pMySQL));
        return STATUS_CODE_FAILURE;
      }

      MYSQL_RES *pMySQLResult = mysql_store_result(m_pMySQL);

      if (!pMySQLResult) {
        dqm_error("MySQL store result failed : {0}", mysql_error(m_pMySQL));
        return STATUS_CODE_FAILURE;
      }

      MYSQL_ROW row = mysql_fetch_row(pMySQLResult);

      if (!dqm4hep::core::stringToType(row[0], result)) {
        mysql_free_result(pMySQLResult);
        return STATUS_CODE_FAILURE;
      }

      mysql_free_result(pMySQLResult);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode DBInterface::queryVector(const std::string &uquery, std::vector<T> &result) {
      if (!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;

      if (mysql_query(m_pMySQL, uquery.c_str())) {
        dqm_error("MySQL query failed : {0}", mysql_error(m_pMySQL));
        return STATUS_CODE_FAILURE;
      }

      MYSQL_RES *pMySQLResult = mysql_store_result(m_pMySQL);

      if (!pMySQLResult) {
        dqm_error("MySQL store result failed : {0}", mysql_error(m_pMySQL));
        return STATUS_CODE_FAILURE;
      }

      MYSQL_ROW row;

      while ((row = mysql_fetch_row(pMySQLResult))) {
        T value = T();

        if (!dqm4hep::core::stringToType(row[0], value))
          continue;

        result.push_back(value);
      }

      mysql_free_result(pMySQLResult);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename Handler>
    inline StatusCode DBInterface::queryAndHandle(const std::string &uquery, Handler handler) {
      if (!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;

      if (mysql_query(m_pMySQL, uquery.c_str())) {
        dqm_error("MySQL query failed : {0}", mysql_error(m_pMySQL));
        return STATUS_CODE_FAILURE;
      }

      MYSQL_RES *pMySQLResult = mysql_store_result(m_pMySQL);

      if (!pMySQLResult) {
        dqm_error("MySQL store result failed : {0}", mysql_error(m_pMySQL));
        return STATUS_CODE_FAILURE;
      }

      handler(pMySQLResult);

      mysql_free_result(pMySQLResult);

      return STATUS_CODE_SUCCESS;
    }
  }
}

#endif //  DQM4HEP_DBINTERFACE_H
