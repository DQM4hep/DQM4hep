/**
 * A javascript client interface to dim web bridge. The provided classes allow
 * to connect to one or many dim web bridge(s), enabling dim service subscription,
 * command sending and rpc handling in javascript code
 *
 * @summary Javascript DIM client interface
 * @author Remi Ete
 * @copyright Remi Ete
 * @since 03.18.2017
 */

var dim = {
  // default web bridge port
  defaultPort : 2511,

  // default web bridge host
  defaultHost : "localhost",

  // time interval to reconnect if service disconnects
  autoReconnect : 2000,

  // types used by DIM
  types : {
    integer : 0,
    float : 1,
    double : 2,
    short : 3,
    longlong : 4,
    string : 5,
    binary : 6
  },

  // websocket routes used by the web bridge
  websockets : {
    browser : {
      getServices : "browser/getServices",
      getServers : "browser/getServers",
      getServerServices : "browser/getServerServices",
      getServerClients : "browser/getServerClients"
    },
    command : "command",
    rpc : "rpc",
    subscribe : "subscribe",
    unsubscribe : "unsubscribe"
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
  return new WebSocket("ws://" + client.host + ":" + client.port + "/" + route);
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

// The global uid generator
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
  var self = this;
  this.host = typeof host === 'undefined' ? dim.defaultHost : host;
  this.port = typeof port === 'undefined' ? dim.defaultPort : port;
  this.errorHandler = null;

  var guid = dim.guid;
  var servicesData = {};

  var ws = dim.createWebSocket(this, "DimClient");
  ws.addEventListener('open', this.onopen);
  ws.addEventListener('message', this.onmessage);
  ws.addEventListener('close', this.onclose);

  this.onopen = function(evt) {
    console.log("Connection to dim web server opened (host : " + self.host + ", port : " + self.port + ") !");
  };

  this.onmessage = function(evt) {
    var result = {};
    try {
      result = JSON.parse(evt.data);
    } catch (e) {
      console.log("Invalid contents received. Expected json string !");
      return;
    }
    var id = result["id"];
    var data = result["data"];

    var serviceData = this.servicesData[id.toString()];

    if(typeof serviceData === 'undefined')
      return;

    serviceData.handler(data);

    if(serviceData.remove)
      this.servicesData[id.toString()] = undefined;
  };

  this.onclose = function(evt) {
    console.log("Connection to dim web server closed (host : " + this.host + ", port : " + this.port + ") !");
  };

  /**
   * Send a dim command
   * @param  {string} commandName the command name
   * @param  {*}      commandData the command data to send
   * @param  {number} commandType the type of command (see dim.types enum)
   */
  this.sendCommand = function(commandName, commandData, commandType) {

  };


  /**
   * Send an integer using a dim command
   *
   * @param  {string} commandName the command name
   * @param  {number} commandData the command data to send
   */
  this.sendIntCommand = function(commandName, commandData) {

    if(typeof commandData !== 'number')
      return;

    this.sendCommand(commandName, commandData, dim.types.integer);
  };


  /**
   * Send a float using a dim command
   *
   * @param  {string} commandName the command name
   * @param  {number} commandData the command data to send
   */
  this.sendFloatCommand = function(commandName, commandData) {

    if(typeof commandData !== 'number')
      return;

    this.sendCommand(commandName, commandData, dim.types.float);
  };


  /**
   * Send a float using a dim command
   *
   * @param  {string} commandName the command name
   * @param  {number} commandData the command data to send
   */
  this.sendDoubleCommand = function(commandName, commandData) {

    if(typeof commandData !== 'number')
      return;

    this.sendCommand(commandName, commandData, dim.types.double);
  };


  /**
   * Send a short integer using a dim command
   *
   * @param  {string} commandName the command name
   * @param  {number} commandData the command data to send
   */
  this.sendShortCommand = function(commandName, commandData) {

    if(typeof commandData !== 'number')
      return;

    this.sendCommand(commandName, commandData, dim.types.short);
  };

  /**
   * Send a long long integer using a dim command
   *
   * @param  {string} commandName the command name
   * @param  {number} commandData the command data to send
   */
  this.sendLongLongCommand = function(commandName, commandData) {

    if(typeof commandData !== 'number')
      return;

    this.sendCommand(commandName, commandData, dim.types.longlong);
  };


  /**
   * Send a string using a dim command
   *
   * @param  {string} commandName the command name
   * @param  {number} commandData the command data to send
   */
  this.sendStringCommand = function(commandName, commandData) {

    if(typeof commandData !== 'string')
      return;

    this.sendCommand(commandName, commandData, dim.types.string);
  };


  /**
   * Send a request (RpcInfo equivalent)
   *
   * @param  {Object}   params           the request parameters
   * @param  {string}   params.rpcName
   * @param  {number}   params.intype
   * @param  {number}   params.outtype
   * @param  {*}        params.rpcData   the rpc data to send
   * @param  {*}        params.rpcNolink the rpc data to receive if the rpc is not available
   * @param  {function} handler          the callback function on rpc answer reception
   */
  this.sendRequest = function(params, handler) {

  };


  /**
   * Get the current dim service content (DimCurrentInfo equivalent)
   *
   * @param  {string}   serviceName the service to query the current content
   * @param  {function} handler     the callback function to receive the service content
   * @param  {*}        nolink      the service data to receive in case the service is not available
   */
  this.getCurrentServiceContent = function(serviceName, handler, nolink) {

  };


  /**
   * Subscribe to dim service (DimInfo equivalent)
   *
   * @param  {Object} metadata the service info to subscribe
   * @param  {string} metadata.name the service name
   * @param  {number} metadata.time the update interval of the dim service (optional)
   * @param  {*}      metadata.nolink the data to receive in case the service is not available
   * @param  {function} handler  the callback function to receive service updates
   */
  this.subscribe = function(metadata, handler) {

  };

  // BROWSER INTERFACE
  /**
   * Get all available services given a pattern.
   *
   * Example :
   *   browser.getServices("RUN_CTRL/*");
   *   browser.getServices("*"); // get all services
   *
   * @param  {string}   serviceName the service pattern
   * @param  {function} handler     the callback function to receive the service list
   */
  this.getServices = function(serviceName, handler) {

  };


  /**
   * Get all available servers on the dim dns node
   *
   * @param  {function} handler the callback function to receive the server list
   */
  this.getServers = function(handler) {

  };


  /**
   * Get all services for a target dim server
   *
   * @param  {string}   serverName the dim server name
   * @param  {function} handler    the callback function to receive the server service list
   */
  this.getServerServices = function(serverName, handler) {

  };


  /**
   * Get all client connected to a target dim server
   *
   * @param  {string}   serverName the dim server name
   * @param  {function} handler    the callback function to receive the client list
   */
  this.getServerClients = function(serverName, handler) {

  };

};





//
