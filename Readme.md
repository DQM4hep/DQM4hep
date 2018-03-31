<img src="https://travis-ci.org/DQM4HEP/dqm4hep-net.svg?branch=master" align="left" />
<img src="https://scan.coverity.com/projects/15305/badge.svg" align="left" />
<img src="https://readthedocs.org/projects/dqm4hep/badge/?version=latest" align="left" />
<a href="https://dqm4hep.github.io/dqm4hep-doxygen/doxygen/dqm4hep-net/master/index.html">
<img src="https://codedocs.xyz/doxygen/doxygen.svg" align="left"/>
</a>

# DQM4HEP Networking layer
# A Data Quality Monitoring for High Energy Physics
Copyright 2017-2018

## INSTALL:

The build process is managed by [CMake](http://cmake.org)

### Dependencies

- [dqm4hep](https://github.com/dqm4hep/dqm4hep) for CMake macros 
- [dqm4hep-core](https://github.com/dqm4hep/dqm4hep-core)

### Install

In the root directory :

```bash
mkdir build
cd build
cmake -DCMAKE_MODULE_PATH=/path/to/dqm4hep/cmake -DDQMCore_DIR=/path/to/DQMCore/ [-DOPTIONS=...] ..
make install
```

where OPTIONS can be :
- INSTALL_DOC [ON/OFF] to install [doxygen](www.doxygen.org) code documentation (default is OFF)
- DQMNET_WEBSOCKETS [ON/OFF] to build websocket interface (default is OFF)
- DIM_GUI [ON/OFF] to build dim GUI (default is OFF)
- DIM_EXAMPLES [ON/OFF] to build dim examples (default id OFF)
- DIM_32BIT [ON/OFF] to build DIM 32 bit support (default is OFF)

### Run Web interface (experimental)

The web interface is a separate process that subscribe to service, send command and requests to dqm servers and forward the reponses via websockets. This enable to connect to dqm servers through this process by using web languages such as javascript or php.

A javascript client is provided to directly connect to this dqm servers.

Please, set the DQMNET_WEBSOCKETS option to ON to enable this feature.

### Bug report

You can send emails to <dqm4hep@gmail.com>
or use the [github issues interface](https://github.com/DQM4HEP/dqm4hep-net/issues)
