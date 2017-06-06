/**
 * A javascript client interface to dqm4hep net web bridge. The provided classes allow
 * to connect to one or many dqm4hep net web bridge(s), enabling dqm4hep service subscription,
 * command sending and rpc handling in javascript code
 *
 * @summary Javascript dqm4hep net client interface
 * @author Remi Ete
 * @copyright Remi Ete
 * @since 03.18.2017
 */

var dim = {
  // default web port
  defaultPort : 2511,

  // default web host
  defaultHost : "localhost",

  // websocket routes
  websockets : {
    browser : {
      getServices : "dqmnet/browser/getServices",
      getServers : "dqmnet/browser/getServers",
      getServerServices : "dqmnet/browser/getServerServices",
      getServerClients : "dqmnet/browser/getServerClients"
    },
    command : "dqmnet/command",
    rpc : "dqmnet/rpc",
    service : "dqmnet/service"
  },

  // The maximum size of service name allowed by dim
  maxServiceName : 132,

  // The maximum size of an integer after string conversion
  maxNumberStrLen : 16,

  // error codes
  errorCodes : {
    serviceNameTooLong : 1,
    invalidUserData : 2,
    noHandlerRegistered : 3,
    websocketNotReady : 4
  }
};

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

/**
 * Utility function to create a websocket
 *
 * @param  {dim.DimClient} client the dim client interface
 * @param  {string}        route  the websocket route
 * @return {WebSocket}            a configured websocket
 */
dim.createWebSocket = function(client, route) {
  return new ReconnectingWebSocket("ws://" + client.host + ":" + client.port + "/" + route);
};

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

/**
 * Utility class to generate a unique identifier.
 * Generated identifiers can be recycled when
 * user no longer needs it using the reUse() function
 */
dim.UID = function() {
  var id = 0;
  var reuseids = [];

  /**
   * Generate unique identifier
   * @return {number} a unique identifier
   */
  this.getUID = function() {
    if(reuseids.length > 0) {
      return reuseids.shift();
    }
    return id++;
  };

  /**
   * Re use an already generated identifier
   * @param  {number} rid the identifier to reuse
   */
  this.reUse = function(rid) {
    if(rid < 0 || id <= rid || typeof rid != 'number' || reuseids.indexOf(rid) >= 0)
      return;
    reuseids.push(rid);
  }
};

// The global unique id generator
dim.guid = new dim.UID();

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

/**
 * Main dim client interface. Connect to a dim web bridge using a websocket
 * and thus can handle service subscription, command sending and rpc handling
 *
 * @param {string} host the web bridge host to connect
 * @param {port}   port the web bridget port to connect
 */
dim.DimClient = function (host, port) {

  // useful variable for anonymous functions
  var self = this;

  // host and port
  this.host = typeof host === 'undefined' ? dim.defaultHost : host;
  this.port = typeof port === 'undefined' ? dim.defaultPort : port;

  // callback functions
  this.errorHandler = null;    ///< user callback function on client error
  this.onconnect = null;       ///< user callback function when all websockets are opened
  this.ondisconnect = null;    ///< user callback function when all websockets are closed

  // utility array to loop easily on all websockets
  this.websockets = [];

  // websockets
  this.commandWs = null;
  this.rpcWs = null;
  this.serviceWs = null;
  this.getServicesWs = null;
  this.getServersWs = null;
  this.getServerServicesWs = null;
  this.getServerClientsWs = null;

  // handlers for rpc, browser and services
  this.uniqueHandlers = {};
  this.serviceHandlers = {};

  // global unique ID generator instance
  var guid = dim.guid;

  /* Helper function for websockets */
  var onopen = function(evt){
    var opencounter = 0;

    for(var i=0 ; i<self.websockets.length ; i++)
      if(self.websockets[i].readyState == 1)
        opencounter++;

    console.log("open counter = " + opencounter.toString());

    if(opencounter == self.websockets.length && self.onconnect != null){
      self.onconnect();
    }
  }

  /* Helper function for websockets */
  var onclose = function(evt){
    var closecounter = 0;

    for(var i=0 ; i<self.websockets.length ; i++)
      if(self.websockets[i].readyState == 3)
        closecounter++;

    if(closecounter == self.websockets.length && self.ondisconnect != null){
      self.ondisconnect();
    }
  }

  /* Helper function for websockets */
  var receiveData = function(evt) {
    var data = evt.data;
    var uidStr = data.substr(0, dim.maxNumberStrLen).trim();
    var userData = data.substr(dim.maxNumberStrLen);
    var handler = self.uniqueHandlers[uidStr];

    if(handler !== undefined) {
      handler(userData);
      delete self.uniqueHandlers[uidStr];
    }

    // keep that uid for later (re)use
    guid.reUse(Number(uidStr));
  };

  /* Create a websocket. Called on client creation */
  this.createWebSocket = function(ws, url){
    if(ws)
      ws.close();
    ws = null;
    return dim.createWebSocket(this, url);
  };

  /* Create all websockets and initialize */
  this.createWebSockets = function() {
    this.commandWs = this.createWebSocket(this.commandWs, dim.websockets.command);
    this.rpcWs = this.createWebSocket(this.rpcWs, dim.websockets.rpc);
    this.serviceWs = this.createWebSocket(this.serviceWs, dim.websockets.service);
    this.getServicesWs = this.createWebSocket(this.getServicesWs, dim.websockets.browser.getServices);
    this.getServersWs = this.createWebSocket(this.getServersWs, dim.websockets.browser.getServers);
    this.getServerServicesWs = this.createWebSocket(this.getServerServicesWs, dim.websockets.browser.getServerServices);
    this.getServerClientsWs = this.createWebSocket(this.getServerClientsWs, dim.websockets.browser.getServerClients);

    this.websockets = [this.commandWs, this.rpcWs, this.serviceWs, this.getServicesWs, this.getServersWs, this.getServerServicesWs, this.getServerClientsWs];

    this.commandWs.onopen = onopen;
    this.rpcWs.onopen = onopen;
    this.getServicesWs.onopen = onopen;
    this.getServersWs.onopen = onopen;
    this.getServerServicesWs.onopen = onopen;
    this.getServerClientsWs.onopen = onopen;

    this.commandWs.onclose = onclose;
    this.rpcWs.onclose = onclose;
    this.serviceWs.onclose = onclose;
    this.getServicesWs.onclose = onclose;
    this.getServersWs.onclose = onclose;
    this.getServerServicesWs.onclose = onclose;
    this.getServerClientsWs.onclose = onclose;

    this.rpcWs.onmessage = receiveData;
    this.getServicesWs.onmessage = receiveData;
    this.getServersWs.onmessage = receiveData;
    this.getServerServicesWs.onmessage = receiveData;
    this.getServerClientsWs.onmessage = receiveData;

    this.serviceWs.onmessage = function(evt) {
      var data = evt.data;
      var serviceName = data.substr(0, dim.maxServiceName).trim();
      var serviceData = data.substr(dim.maxServiceName);
      var handlers = self.serviceHandlers[serviceName];

      if(handlers !== undefined)
        for(var i=0 ; i<handlers.length ; i++)
          handlers[i](serviceData);
    };

    this.serviceWs.onopen = function(evt){
      onopen();
    }
  }

  var addServiceHandler = function(serviceName, handler) {
    var handlers = self.serviceHandlers[serviceName];

    if(handlers === undefined)
      self.serviceHandlers[serviceName] = [];

    if(self.serviceHandlers[serviceName].find(handler) == undefined)
      self.serviceHandlers[serviceName].push(handler);
  }

  var removeServiceHandler = function(serviceName, handler) {
    var handlers = self.serviceHandlers[serviceName];

    if(handlers !== undefined) {
      for(var i=0 ; i<handlers.length ; i++) {
        if(handlers[i] == handler) {
          handlers.splice(i, 1);
          return;
        }
      }
    }
  }

  /**
   * Whether the wbsockets are ready
   * @return {bool}
   */
  this.websocketsReady = function() {
    for(var i=0; i<this.websockets.length ; i++)
      if(this.websockets[i].readyState != 1)
        return false;
    return true;
  }

  /**
   * Print all websocket status in console
   */
  this.printWsStatus = function() {
    for(var i=0; i<this.websockets.length ; i++)
      console.log(this.websockets[i].readyState);
  };


  /**
   * Send a command
   * @param  {string} commandName the command name
   * @param  {*}      commandData the command data to send
   */
  this.sendCommand = function(commandName, commandData) {
    if(commandName.length >= dim.maxServiceName) {
      if(this.errorHandler !== null) {
        this.errorHandler(dim.errorCodes.serviceNameTooLong);
        return;
      }
    }

    if(this.commandWs.readyState != 1) {
      if(this.errorHandler !== null) {
        this.errorHandler(dim.errorCodes.websocketNotReady);
        return;
      }
    }

    var message = commandName;
    message += " ".repeat(dim.maxServiceName-commandName.length);

    if(typeof commandData === 'string') {
      message += commandData;
    }
    else if(typeof commandData.toString === 'function') {
      message += commandData.toString();
    }
    else if(typeof commandData === 'object') {
      message += JSON.stringify(commandData);
    }
    else if(this.errorHandler !== null) {
      this.errorHandler(dim.errorCodes.invalidUserData);
      return;
    }

    this.commandWs.send(message);
  };


  /**
   * Send a request (RpcInfo equivalent)
   *
   * @param  {string}   rpcName
   * @param  {*}        rpcData          the rpc data to send
   * @param  {function} handler          the callback function on rpc answer
   */
  this.sendRequest = function(rpcName, rpcData, handler) {
    if(rpcName.length >= dim.maxServiceName) {
      if(this.errorHandler !== null) {
        this.errorHandler(dim.errorCodes.serviceNameTooLong);
        return;
      }
    }

    var message += rpcName;
    message += " ".repeat(dim.maxServiceName-rpcName.length);

    var rpcUid = guid.getUID().toString();

    // write rpc uid
    message += rpcUid;
    message += " ".repeat(dim.maxNumberStrLen-rpcUid.length);

    if(typeof rpcData === 'string') {
      message += rpcData;
    }
    else if(rpcData.toString === 'function') {
      message += rpcData.toString();
    }
    else if(typeof rpcData === 'object') {
      message += JSON.stringify(rpcData);
    }
    else if(this.errorHandler !== null) {
      this.errorHandler(dim.errorCodes.invalidUserData);
      return;
    }

    // register handler on rpc reception
    this.uniqueHandlers[rpcUid] = handler;

    // send rpc data
    this.rpcWs.send(message);
  };


  /**
   * Subscribe to service
   *
   * @param  {string} serviceName the service name
   * @param  {function} handler  the callback function to receive service updates
   */
  this.subscribe = function(serviceName, handler) {
    if(serviceName.length >= dim.maxServiceName) {
      if(this.errorHandler !== null) {
        console.log("Error caught from subscribe");
        this.errorHandler(dim.errorCodes.serviceNameTooLong);
        return;
      }
    }

    var message = serviceName;
    message += " ".repeat(dim.maxServiceName-serviceName.length);
    message += "subscribe";

    this.addServiceHandler(serviceName, handler);
    this.serviceWs.send(message);
  };


  /**
   * Un-subscribe from dim service (DimInfo equivalent)
   *
   * @param  {string} serviceName the service name
   * @param  {function} handler  the callback function to receive service updates
   */
  this.unsubscribe = function(serviceName, handler) {
    if(serviceName.length >= dim.maxServiceName) {
      if(this.errorHandler !== null) {
        console.log("Error caught from unsubscribe");
        this.errorHandler(dim.errorCodes.serviceNameTooLong);
        return;
      }
    }

    var message = serviceName;
    message += " ".repeat(dim.maxServiceName-serviceName.length);
    message += "unsubscribe";

    this.removeServiceHandler(serviceName, handler);
    this.serviceWs.send(message);
  };

  // BROWSER INTERFACE
  /**
   * Get all available services given a pattern.
   *
   * Example :
   *   client.getServices("RUN_CTRL/*", function(services) { console.log(services) });
   *   client.getServices("*", function(services) { console.log(services) }); // get all services
   *
   * @param  {string}   serviceName the service pattern
   * @param  {function} handler     the callback function to receive the service list (Array)
   */
  this.getServices = function(serviceName, handler) {
    var uid = guid.getUID().toString();
    var message = uid;
    message += " ".repeat(dim.maxNumberStrLen-uid.length);
    message += serviceName;

    this.uniqueHandlers[uid] = function(data) {
      var jsonData = JSON.parse(data);
      var services = jsonData["services"];
      handler(services);
    };

    this.getServicesWs.send(message);
  };


  /**
   * Get all available servers
   *
   * @param  {function} handler the callback function to receive the server list (Array)
   */
  this.getServers = function(handler) {
    var uid = guid.getUID().toString();
    var message = uid;
    message += " ".repeat(dim.maxNumberStrLen-uid.length);

    this.uniqueHandlers[uid] = function(data) {
      var jsonData = JSON.parse(data);
      var servers = jsonData["servers"];
      handler(servers);
    };

    this.getServersWs.send(message);
  };


  /**
   * Get all services for a target server
   *
   * @param  {string}   serverName the server name
   * @param  {function} handler    the callback function to receive the server service list
   */
  this.getServerServices = function(serverName, handler) {
    var uid = guid.getUID().toString();
    var message = uid;
    message += " ".repeat(dim.maxNumberStrLen-uid.length);
    message += serverName;

    this.uniqueHandlers[uid] = function(data) {
      var jsonData = JSON.parse(data);
      var serverServices = jsonData["serverServices"];
      handler(serverServices);
    };

    this.getServerServicesWs.send(message);
  };


  /**
   * Get all client connected to a target server
   *
   * @param  {string}   serverName the server name
   * @param  {function} handler    the callback function to receive the client list
   */
  this.getServerClients = function(serverName, handler) {
    var uid = guid.getUID().toString();
    var message = uid;
    message += " ".repeat(dim.maxNumberStrLen-uid.length);
    message += serverName;

    this.uniqueHandlers[uid] = function(data) {
      var jsonData = JSON.parse(data);
      var serverClients = jsonData["serverClients"];
      handler(serverClients);
    };

    this.getServerClientsWs.send(message);
  };

  // init the client
  this.createWebSockets();
};


//
