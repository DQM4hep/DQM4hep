[![Build Status](https://travis-ci.org/DQM4HEP/dqm4hep-net.svg?branch=master)](https://travis-ci.org/DQM4HEP/dqm4hep-net)

# DQM4HEP Networking layer
# A Data Quality Monitoring for High Energy Physics
Copyright IPNL/CNRS/IN2P3

## INSTALL:

The building process is managed by [CMake](http://cmake.org)

### Dependencies

2 dependencies :
* [DIM](https://dim.web.cern.ch/dim/) for networking communcations
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) that can installed via the github repo or using apt like tools (package libjsoncpp-dev)


### install

In the root directory :

```bash
mkdir build
cd build
cmake -DDIMDIR=/path/to/dimdir/ [-DOPTIONS=...] ..
make
```

where OPTIONS can be :
* INSTALL_DOC [ON/OFF] to install [doxygen](www.doxygen.org) code documentation (default is OFF)

### Bug report

You can send emails to <dqm4hep@gmail.com>
or use the [github issues interface](https://github.Com/DQM4HEP/dqm4hep-net/issues)
