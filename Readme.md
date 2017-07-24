[![Build Status](https://travis-ci.org/DQM4HEP/dqm4hep-net.svg?branch=master)](https://travis-ci.org/DQM4HEP/dqm4hep-net)

# DQM4HEP Networking layer
# A Data Quality Monitoring for High Energy Physics
Copyright 2017

## INSTALL:

The building process is managed by [CMake](http://cmake.org)

### Dependencies

Only one dependency :
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) that can installed via the github repo or using apt like tools (package libjsoncpp-dev)

### Install

In the root directory :

```bash
mkdir build
cd build
cmake [-DOPTIONS=...] ..
make
```

where OPTIONS can be :
- INSTALL_DOC [ON/OFF] to install [doxygen](www.doxygen.org) code documentation (default is OFF)
- DQMNET_WEBSOCKETS [ON/OFF] to build websocket interface (default is OFF)
- DIM_GUI [ON/OFF] to build dim GUI (default is OFF)
- DIM_EXAMPLES [ON/OFF] to build dim examples (default id OFF)
- DIM_32BIT [ON/OFF] to build DIM 32 bit support (default is OFF)

### Run Web interface

The web interface is a separate process that subscribe to service, send command and requests to dqm servers and forward the reponses via websockets. This enable to connect to dqm servers through this process by using web languages such as javascript or php.

A javascript client is provided to directly connect to this dqm servers.

Please, set the DQMNET_WEBSOCKETS option to ON to enable this feature.

### Bug report

You can send emails to <dqm4hep@gmail.com>
or use the [github issues interface](https://github.Com/DQM4HEP/dqm4hep-net/issues)
