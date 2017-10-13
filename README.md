[![Build Status](https://travis-ci.org/DQM4HEP/dqm4hep.svg?branch=master)](https://travis-ci.org/DQM4HEP/dqm4hep)
[![Coverity Scan](https://scan.coverity.com/projects/13354/badge.svg)](https://scan.coverity.com/projects/dqm4hep-dqm4hep)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1011114.svg)](https://doi.org/10.5281/zenodo.1011114)

# DQM4HEP
# A Data Quality Monitoring for High Energy Physics
DQM4HEP is distributed under the [GPLv3 License](http://www.gnu.org/licenses/gpl-3.0.en.html)

[![License](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl-3.0.en.html)

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

### CVMFS based installation (lxplus, server connected to the cern grid, etc.)
Most of the dependencies are already available over cvmfs. Only Log4cxx and updated version of apr and apr-util libraries are missing (but will be automatically installed if needed).

First you have to set up an updated version of cmake:
```bash
 export PATH=/cvmfs/sft.cern.ch/lcg/contrib/CMake/3.6.0/Linux-x86_64/bin:${PATH}
```
and gcc (for c++11 compliance):
```bash
GCC_version=4.8
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/${GCC_version}/x86_64-slc6/setup.sh
```

A cmake configuration file is available with properly configured version of root6, boost1.59 and qt4.8.4 from the lcg release installation. Depending on the environment you might also have to unset the QTDIR variable prior to compilation.
```bash
mkdir build
cd build
unset QTDIR
cmake [-DOPTIONS=..] -C ../cvmfsInstall.cmake ..
make -jN
```

If you plan on installing DQM4ILC, you can use the ILCSoft installation available at /cvmfs/ilc.desy.de/sw/. As of this writing the latest version available is v1.17.11 :

```bash
export ILCSOFT=/cvmfs/ilc.desy.de/sw/x86_64_gcc48_sl6/v01-17-11
mkdir build
cd build
cmake [-DOPTIONS=..] -C $ILCSOFT/ILCSoft.cmake -C ../cvmfsInstall.cmake ..
make -jN
```

### Known bugs

#### When compiling DQM4ILC
Some combination of ilcutil and gcc version produces error with the default CXX_FLAGS from ilcutil.
This was resolved in commit https://github.com/iLCSoft/iLCUtil/commit/2c9d897bff08f7e98f4c68dee4f5bc57c8df97e9.
Either use ilcutil v01-05 (not shipped as of this writing) or replace the `cmakemodules/ilcsoft_default_cxx_flags.cmake` file from your ilcutil folder by the one associtaed with commit https://github.com/iLCSoft/iLCUtil/commit/2c9d897bff08f7e98f4c68dee4f5bc57c8df97e9.

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
