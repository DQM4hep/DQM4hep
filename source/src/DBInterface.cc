/// \file DBInterface.cc
/*
 *
 * DBInterface.cc source template automatically generated by a class generator
 * Creation date : lun. janv. 11 2016
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */

// -- dqm4hep headers
#include <dqm4hep/DBInterface.h>

// -- std headers
#include <fstream>
#include <iomanip>

namespace dqm4hep {

  namespace core {

    DBInterface::DBInterface() :
        m_pMySQL(nullptr),
        m_isConnected(false)
    {

    }

    //-------------------------------------------------------------------------------------------------

    DBInterface::DBInterface(const std::string &host, const std::string &user, const std::string &password, const std::string &database) :
		    m_pMySQL(nullptr),
		    m_isConnected(false)
    {
      this->set(host, user, password, database);
      this->connect();
    }

    //-------------------------------------------------------------------------------------------------

    DBInterface::~DBInterface()
    {
      if(this->isConnected())
        this->disconnect();
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DBInterface::connect()
    {
      if(this->isConnected())
        return STATUS_CODE_ALREADY_INITIALIZED;

      try
      {
        // create mysql instance
        m_pMySQL = mysql_init(nullptr);

        if(m_pMySQL == nullptr)
        {
          dqm_error( "Couldn't create mysql instance : {0}", mysql_error(m_pMySQL) );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        // create connection to database
        if(nullptr == mysql_real_connect(m_pMySQL, m_host.c_str(), m_user.c_str(), m_password.empty() ? nullptr : m_password.c_str(),
            nullptr, 0, nullptr, 0))
        {
          dqm_error( "Couldn't initialize mysql connection : {0}", mysql_error(m_pMySQL) );
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        m_isConnected = true;

        if(!m_database.empty())
        {
          // select data base
          std::stringstream query;
          query << "USE " << m_database << " ;";

          THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->execute(query.str()));          
        }
      }
      catch(const StatusCodeException &exception)
      {
        if(m_pMySQL != nullptr)
          mysql_close(m_pMySQL);

        m_isConnected = false;

        return exception.getStatusCode();
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DBInterface::disconnect()
    {
      if(this->isConnected())
        return STATUS_CODE_UNCHANGED;

      if(m_pMySQL != nullptr)
        mysql_close(m_pMySQL);

      m_isConnected = false;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DBInterface::connect(const std::string &host, const std::string &user, const std::string &password, const std::string &database)
    {
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->set(host, user, password, database));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->connect());

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DBInterface::set(const std::string &host, const std::string &user, const std::string &password, const std::string &database)
    {
      if(this->isConnected())
        return STATUS_CODE_NOT_ALLOWED;

      m_host = host;
      m_user = user;
      m_password = password;
      m_database = database;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &DBInterface::getHost() const
    {
      return m_host;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &DBInterface::getUser() const
    {
      return m_user;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &DBInterface::getPassword() const
    {
      return m_password;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &DBInterface::getDataBase() const
    {
      return m_database;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DBInterface::queryRaw(const std::string &query, void *&pResult)
    {
      pResult = nullptr;

      if(!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;

      if(mysql_query(m_pMySQL, query.c_str()) != 0)
      {
        dqm_error( "MySQL query failed : {0}", mysql_error(m_pMySQL) );
        return STATUS_CODE_FAILURE;
      }

      MYSQL_RES *pMySQLResult = mysql_store_result(m_pMySQL);

      if(pMySQLResult == nullptr)
      {
        dqm_error( "MySQL store result failed : {0}", mysql_error(m_pMySQL) );
        return STATUS_CODE_FAILURE;
      }

      MYSQL_ROW row = mysql_fetch_row(pMySQLResult);

      pResult = (void *) row[0];

      mysql_free_result(pMySQLResult);

      if(nullptr == pResult)
        return STATUS_CODE_FAILURE;

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DBInterface::execute(const std::string &query)
    {
      if(!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;

      if(mysql_query(m_pMySQL, query.c_str()) != 0)
      {
        dqm_error( "MySQL query failed : {0}", mysql_error(m_pMySQL) );
        return STATUS_CODE_FAILURE;
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    bool DBInterface::isConnected()
    {
      return m_isConnected;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DBInterface::createParameterTable(const std::string &table, bool ifNotExists, bool dropExistingTable)
    {
      if(!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;
        
      if(table.empty())
        return STATUS_CODE_INVALID_PARAMETER;
        
      if(dropExistingTable)
      {
        std::stringstream query;
        query << "DROP TABLE IF EXISTS " << table << ";";
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->execute(query.str()));
      }
      
      std::stringstream query;
      query << "CREATE TABLE ";

      if(ifNotExists)
        query << " IF NOT EXISTS ";

      query << table
            << " ( "
            << "parameter VARCHAR(256) NOT NULL,"
            << "value MEDIUMTEXT NOT NULL,"
            << "last_update DATETIME NOT NULL,"
            << "PRIMARY KEY (parameter) ) ENGINE=INNODB;"; 
      
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->execute(query.str()));
      
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    StatusCode DBInterface::emptyParameterTable(const std::string &table)
    {
      if(!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;
        
      if(table.empty())
        return STATUS_CODE_INVALID_PARAMETER;
        
      return this->execute("TRUNCATE TABLE " + table + " ;");
    }
    
    //-------------------------------------------------------------------------------------------------
    
    StatusCode DBInterface::setParameters(const std::string &table, const StringMap &parameterValueMap)
    {
      if(!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;
        
      if(table.empty() || parameterValueMap.empty())
        return STATUS_CODE_INVALID_PARAMETER;
      
      std::stringstream query;
      query << "REPLACE INTO " 
            << table
            << " (parameter, value, last_update) VALUES ";
      
      bool firstEntry = true;
      
      for(auto pvPair : parameterValueMap)
      {
        std::stringstream entry;
        entry << "( \"" << pvPair.first << "\", \"" << pvPair.second << "\", NOW()) ";
        
        if(!firstEntry)
          query << " , ";
        
        query << entry.str();
        firstEntry = false;
      }
      
      query << " ;";
      
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->execute(query.str()));
      
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    StatusCode DBInterface::backupParameterTable(const std::string &table, const std::string &backupTable)
    {
      if(!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;
        
      if(table.empty() || backupTable.empty())
        return STATUS_CODE_INVALID_PARAMETER;
      
      std::stringstream query;
      query << "CREATE TABLE " << backupTable << " LIKE " << table << "; INSERT " << backupTable << " SELECT * FROM " << table << ";";
      return this->execute(query.str());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    StatusCode DBInterface::dumpParameterTable(const std::string &table)
    {
      if(!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;
        
      if(table.empty())
        return STATUS_CODE_INVALID_PARAMETER;
        
      std::stringstream query;
      query << "SELECT parameter, value, last_update FROM " << table << ";";
            
      return this->queryAndHandle(query.str(), [](MYSQL_RES *result){
        
        std::cout << std::setw(50) << std::left << "Parameter" << std::setw(50) << std::left << "Value" << std::setw(20) << std::left << "Last update" << std::endl;
        std::cout << std::string(120, '-') << std::endl;
        
        int num_fields = mysql_num_fields(result);      
        MYSQL_ROW row;
      
        while ((row = mysql_fetch_row(result)) != nullptr)
          std::cout << std::setw(50) << std::left << row[0] << std::setw(50) << std::left << row[1] << std::setw(20) << std::left << row[2] << std::endl;
      });
    }
    
    //-------------------------------------------------------------------------------------------------
    
    StatusCode DBInterface::getTableParameters(const std::string &table, StringMap &parameterValueMap)
    {
      if(!this->isConnected())
        return STATUS_CODE_NOT_INITIALIZED;
        
      if(table.empty())
        return STATUS_CODE_INVALID_PARAMETER;
        
      std::stringstream query;
      query << "SELECT parameter, value FROM " << table << ";";
            
      return this->queryAndHandle(query.str(), [&parameterValueMap](MYSQL_RES *result){
        
        int num_fields = mysql_num_fields(result);      
        MYSQL_ROW row;
      
        while ((row = mysql_fetch_row(result)) != nullptr)
          parameterValueMap[row[0]] = row[1];
      });
    }

  }

}
