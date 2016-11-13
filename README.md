[![Build Status](https://travis-ci.org/DQM4HEP/DQM4HEP.svg?branch=master)](https://travis-ci.org/DQM4HEP/DQM4HEP)

# DQM4HEP 
# A Data Quality Monitoring for High Energy Physics
Copyright IPNL/CNRS/IN2P3

## INSTALL:

Can be built standalonly or using [ilcsoft](http://ilcsoft.desy.de/portal).

The install procedure is managed by [CMake](http://cmake.org)

### Dependencies

All the needed packages are installed on-the-fly by CMake, except two of them :
* [ROOT](http://root.cern.ch). If you want to build the graphical user interfaces (DQMViz), then **you need to compile ROOT with the flag --enable-qt**
* [Qt](www.qt.io). Only if you want to build the package with DQMViz

Note that these two packages are provided by ilcsoft.

### Standalone install

In the root directory :

```bash
mkdir build
cd build
cmake [-DOPTIONS=...] ..
make
```

where OPTIONS can be :
* BUILD_DQMVIZ [ON/OFF] to install visualization package
* BUILD_DQM4ILC [ON/OFF] to install ilcsoft related support (lcio streamer, marlin interface , etc ...)
* BUILD_EXAMPLES [ON/OFF] to build example implementations
* INSTALL_DOC [ON/OFF] to install [doxygen](www.doxygen.org) code documentation
* DIM_GUI [ON/OFF] to build dim gui support (may not work on all operating systems)

Example :

```bash
cmake -DBUILD_EXAMPLES=ON -DBUILD_DQMVIZ=ON ..
```

All options area by default set to OFF

### Install with ilcsoft

```bash
mkdir build
cd build
cmake [-DOPTIONS=..] -C $ILCSOFT/ILCSoft.cmake ..
make
```

### Known bugs

#### On MAC-OSX:
if you encounter this error when compiling did (dim sub-package) :  
	"./src/did/did.h:1:10: fatal error: 'Mrm/MrmAppl.h' file not found"  
Check if OpenMotif is installed in /usr/OpenMotif  
If it’s not change MOTIFINC and MOTIFLIB variables in makefile_did accordingly  
Eg if using macport:  
	MOTIFINC = /opt/local/include  
	MOTIFLIB = /opt/local/lib  

### Bug report

You can send emails to <dqm4hep@gmail.com>
or use the [github issues interface](https://github.Com/DQM4HEP/DQM4HEP/issues)
