/// \file Service.h
/*
 *
 * Service.h header template automatically generated by a class generator
 * Creation date : sam. d�c. 3 2016
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


#ifndef SERVICE_H
#define SERVICE_H

// -- std headers
#include <string>
#include <typeinfo>

// -- dim headers
#include "dis.hxx"

// -- xdrstream headers
#include "xdrstream/xdrstream.h"

// -- json headers
#include "json/json.h"

namespace dqm4hep {

  namespace net {

    class Server;

    /** Service class (server side component)
     *
     *  Base interface for service. Service is updated whenever
     *  the user calls the update() method. Service content
     *  depends on concrete implementation. The content is filled
     *  on update() call that concrete classes fill via the
     *  writeContent() callback method.
     */
    class Service
    {
      friend class Server;
    public:
      /** Update the service, sending its content to listening clients
       */
      template <typename T>
      void update(const T &value);

      /** Update the service, sending its content to listening clients
       */
      void update(const xdrstream::BufferDevice *pDevice);

      /** Get the service type
       */
      const std::string &getType() const;

      /** Get the service name
       */
      const std::string &getName() const;

      /** Get the service full name, as it is allocated on the network
       */
      const std::string &getFullName() const;

      /** Get the full service name from the service type and name
       */
      static std::string getFullServiceName(const std::string &type, const std::string &name);

      /** Get the server in which the service is declared
       */
      Server *getServer() const;

    protected:
      /** Constructor with service type and name
       */
      Service(Server *pServer, const std::string &type, const std::string &name);

      /** Destructor
       */
      virtual ~Service();

    private:
      std::string           m_type;             ///< The service type
      std::string           m_name;             ///< The service name
      std::string           m_fullName;         ///< The service full name
      std::string           m_serviceContent;   ///< The service string content
      DimService            m_service;          ///< The dim service implementation
      Server               *m_pServer;          ///< The server in which the service is declared
    };

  }

}

#endif  //  SERVICE_H
