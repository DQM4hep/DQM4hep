/// \file OnlineRoutes.cc
/*
 *
 * OnlineRoutes.cc source template automatically generated by a class generator
 * Creation date : mar. mars 22 2016
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


#include <dqm4hep/OnlineManagerServer.h>
#include <dqm4hep/OnlineRoutes.h>
#include <dqm4hep/Logging.h>

namespace dqm4hep {

  namespace online {

    void OnlineManagerServer::run() {
      if(not m_logFileBaseName.empty()) {
        m_logger = core::Logger::createLogger("online-logger", {
          core::Logger::rotatingFile(
            m_logFileBaseName,
            m_logFileMaxSize,
            m_logFileNFiles
          )
        });
        m_logger->set_pattern("%v");
        m_logger->set_level(spdlog::level::trace); // all message        
      }

      m_server = std::make_shared<net::Server>(OnlineRoutes::OnlineManager::serverName());
      
      m_logsService = m_server->createService(OnlineRoutes::OnlineManager::logs());
      m_appStatsService = m_server->createService(OnlineRoutes::OnlineManager::appStats());
      
      m_server->createCommandHandler(OnlineRoutes::OnlineManager::collectLog(), this, &OnlineManagerServer::collectLog);
      m_server->createCommandHandler(OnlineRoutes::OnlineManager::collectAppStat(), this, &OnlineManagerServer::collectAppStat);
      
      m_server->start();
      
      while(not m_stopFlag.load())
        core::sleep(std::chrono::seconds(1));
    }
    
    //-------------------------------------------------------------------------------------------------

    void OnlineManagerServer::stop() {
      m_stopFlag = true;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void OnlineManagerServer::setLogProperties(const std::string &fileBaseName, size_t maxFileSize, size_t maxNFiles) {
      m_logFileBaseName = fileBaseName;
      m_logFileMaxSize = maxFileSize;
      m_logFileNFiles = maxNFiles;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void OnlineManagerServer::collectLog(const net::Buffer &buffer) {
      core::json message;
      try {
        message = core::json::parse(buffer.begin(), buffer.end());
      }
      catch(core::json::parse_error& e) {
        dqm_warning( "OnlineManagerServer::collectLog: received unexpected log message (json parse error)" );
        return;
      }
      // check log consistency
      bool logConsistent = (
        1 == message.count("logger") &&
        1 == message.count("level") &&
        1 == message.count("message") &&
        1 == message.count("host") &&
        1 == message.count("pid")
      );
      if(not logConsistent) {
        dqm_warning( "OnlineManagerServer::collectLog: received inconsistent log message" );
      }
      if(nullptr != m_logger) {
        this->logMessage(message);
      }
      // send the log message to listeners
      m_logsService->sendBuffer(buffer.begin(), buffer.size());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void OnlineManagerServer::collectAppStat(const net::Buffer &buffer) {
      core::json appStat;
      try {
        appStat = core::json::parse(buffer.begin(), buffer.end());
      }
      catch(core::json::parse_error& e) {
        dqm_warning( "OnlineManagerServer::collectAppStat: received unexpected app stat (json parse error)" );
        return;
      }
      // check app stat consistency
      bool appStatConsistent = (
        1 == appStat.count("logger") &&
        1 == appStat.count("value") &&
        1 == appStat.count("appType") &&
        1 == appStat.count("appName") &&
        1 == appStat.count("time")
      );
      if(not appStatConsistent) {
        dqm_warning( "OnlineManagerServer::collectAppStat: received inconsistent app stat" );
      }
      // send the log message to listeners
      m_appStatsService->sendBuffer(buffer.begin(), buffer.size());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void OnlineManagerServer::logMessage(const core::json &message) {
      // extract metadata
      std::string loggerName = message.value<std::string>("logger", "");
      core::Logger::Level logLevel = static_cast<core::Logger::Level>(message.value<int>("level", 0));
      std::string logLevelStr = core::Logger::logLevelToString(logLevel);
      std::string msg = message.value<std::string>("message", "");
      std::string host = message.value<std::string>("host", "");
      int pid = message.value<int>("pid", 0);
      // log message
      dqm_logger_log(m_logger, logLevel, 
        "[{0} on {1} ({2})] {3} - {4}: {5}", 
        loggerName, 
        host, 
        pid,
        currentTimeToString(),
        logLevelStr, 
        msg);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    std::string OnlineManagerServer::currentTimeToString() const {
      std::time_t t = std::time(nullptr);
      std::string currentTime;
      currentTime.resize(9);
      std::strftime(const_cast<char*>(currentTime.c_str()), 9, "%T", std::localtime(&t));
      return currentTime;
    }
    
  }

} 
