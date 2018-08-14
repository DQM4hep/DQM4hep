/// \file EventSource.cc
/*
 *
 * EventSource.cc source template automatically generated by a class generator
 * Creation date : mar. sept. 8 2015
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
#include <dqm4hep/EventSource.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/Server.h>
#include <dqm4hep/OnlineRoutes.h>

namespace dqm4hep {

  namespace online {

    EventSourcePtr EventSource::make_shared(const std::string &sourceName) {
      return std::shared_ptr<EventSource>(new EventSource(sourceName));
    }

    //-------------------------------------------------------------------------------------------------

    EventSource::EventSource(const std::string &name) :
        m_sourceName(name) {
    }
    
    //-------------------------------------------------------------------------------------------------
    
    EventSource::~EventSource() {
      for(auto colIter : m_collectorInfos) {
        this->unregisterMe(colIter.first);
      }
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &EventSource::sourceName() const {
      return m_sourceName;
    }
    
    //-------------------------------------------------------------------------------------------------

    void EventSource::addCollector(const std::string &name) {
      if(m_started) {
        throw core::StatusCodeException(core::STATUS_CODE_NOT_ALLOWED);
      }
      
      auto findIter = m_collectorInfos.find(name);
      
      if(m_collectorInfos.end() != findIter) {
        throw core::StatusCodeException(core::STATUS_CODE_ALREADY_PRESENT);
      }
      
      CollectorInfo info;
      info.m_registered = false;
      m_collectorInfos.insert(CollectorInfoMap::value_type(name, info));
    }

    //-------------------------------------------------------------------------------------------------
    
    void EventSource::start() {
      if(m_started) {
        throw core::StatusCodeException(core::STATUS_CODE_ALREADY_INITIALIZED);
      }
      
      if(m_collectorInfos.empty()) {
        dqm_error( "EventSource::start(): Event source '{0}' has no collector to send event. Couldn't initialize!", m_sourceName );
        throw core::StatusCodeException(core::STATUS_CODE_NOT_INITIALIZED);
      }
      
      m_bufferDevice = std::make_shared<xdrstream::BufferDevice>(2*1024*1024);
      
      core::json sourceInfo;
      this->getSourceInfo(sourceInfo);
      
      for(auto colIter : m_collectorInfos) {
        bool registered = this->registerMe(colIter.first, sourceInfo);
        colIter.second.m_registered = registered;
      }
      
      m_started = true;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventSource::sendEvent(core::EventPtr event) {
      core::StringVector collectors;

      for(auto iter : m_collectorInfos)
        collectors.push_back(iter.first);
        
      this->sendEvent(collectors, event);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventSource::sendEvent(const std::string &collector, core::EventPtr event) {
      if(m_collectorInfos.end() == m_collectorInfos.find(collector)) {
        dqm_error( "EventSource::sendEvent(col,evt): collector '{0}' not registered !", collector );
        throw core::StatusCodeException(core::STATUS_CODE_NOT_FOUND);
      }
      
      this->sendEvent(core::StringVector({collector}), event);
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventSource::sendEvent(const core::StringVector &collectors, core::EventPtr event) {
      if(not m_started) {
        throw core::StatusCodeException(core::STATUS_CODE_NOT_INITIALIZED);
      }
        
      if(nullptr == event) {
        throw core::StatusCodeException(core::STATUS_CODE_INVALID_PTR);
      }
      
      (void)m_bufferDevice->reset();
      THROW_RESULT_IF(core::STATUS_CODE_SUCCESS, !=, m_eventStreamer.writeEvent(event, m_bufferDevice.get()));
      
      core::json sourceInfo;
      net::Buffer collectBuffer;
      auto model = collectBuffer.createModel();
      collectBuffer.setModel(model);
      model->handle(m_bufferDevice->getBuffer(), m_bufferDevice->getPosition());
      
      // send serialized event to all collectors 
      for(auto collector : collectors) {
        auto iter = m_collectorInfos.find(collector);
        
        if(m_collectorInfos.end() == iter) {
          dqm_error( "EventSource::sendEvent(): Collector '{0}' not found... Something is wrong !!!", collector );
          throw core::StatusCodeException(core::STATUS_CODE_NOT_FOUND);
        }
        
        // Is the event collector server actually running ?
        const std::string serverName(OnlineRoutes::Application::serverName(OnlineRoutes::EventCollector::applicationType(), iter->first));
        if(!net::Server::isServerRunning(serverName)) {
          // in case the server was stopped, we will need to send 
          // back the source registration on wake up
          iter->second.m_registered = false;
          continue;
        }
        
        // if not yet registered to the collector, do it
        if(!iter->second.m_registered) {
          // get it once
          if(sourceInfo.empty()) {
            this->getSourceInfo(sourceInfo);
          }            
          iter->second.m_registered = this->registerMe(iter->first, sourceInfo);
        }
        
        if(!iter->second.m_registered) {
          dqm_warning( "EventSource::sendEvent(): Source '{0}' not registered yet to collector '{1}'. Skipping ...", m_sourceName, collector );
          continue;
        }
        
        m_client.sendCommand(OnlineRoutes::EventCollector::collectEvent(collector), collectBuffer);
      }  
    }

    //-------------------------------------------------------------------------------------------------

    void EventSource::getSourceInfo(core::json &info) {
      // host info
      core::StringMap hostInfo;
      core::fillHostInfo(hostInfo);
                  
      // collectors
      core::json collectorsValue;
      for(auto colIter : m_collectorInfos)
        collectorsValue.push_back(colIter.first);
      
      info = {
        {"source", m_sourceName},
        {"host", hostInfo},
        {"collectors", collectorsValue}
      };
    }
    
    //-------------------------------------------------------------------------------------------------
    
    bool EventSource::registerMe(const std::string &collector, const core::json &info) {
      std::string requestName = OnlineRoutes::EventCollector::registerSource(collector);
      bool returnValue(false);
      net::Buffer requestBuffer;
      auto model = requestBuffer.createModel<std::string>();
      requestBuffer.setModel(model);
      std::string jsonDump(info.dump());
      model->move(std::move(jsonDump));
      
      dqm_debug( "Sending request to collector {0} for registration, request: {1}", collector , requestName);
      m_client.sendRequest(requestName, requestBuffer, [&returnValue,&collector](const net::Buffer &buffer){
        core::json response({});
        
        if(0 != buffer.size()) {
          response = core::json::parse(buffer.begin(), buffer.end());
        }
        const bool registered(response.value<bool>("registered", false));
        
        if(not registered) {
          const std::string message(response.value<std::string>("message", "The event collector is maybe not available or is down !"));
          dqm_warning( "EventSource::registerMe(): Couldn't register source to collector '{0}': {1}", collector, message );
          returnValue = false;
        }
        else {
          dqm_info( "Event source registered to event collector '{0}' !", collector );
          returnValue = true;
        }
      });
      
      if(returnValue) {
        dqm_info( "Will notify server on exit" );
        const std::string serverName(OnlineRoutes::Application::serverName(OnlineRoutes::EventCollector::applicationType(), collector));
        m_client.notifyServerOnExit(serverName);        
      }      
      return returnValue;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    void EventSource::unregisterMe(const std::string &collector) {
      std::string commandName = OnlineRoutes::EventCollector::unregisterSource(collector);
      m_client.sendCommand(commandName, std::string(""));
    }

  }

}

