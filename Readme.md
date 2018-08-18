[![Build Status](https://travis-ci.org/DQM4hep/dqm4hep.svg?branch=master)](https://travis-ci.org/DQM4hep/dqm4hep)
[![Coverity Scan](https://scan.coverity.com/projects/13354/badge.svg)](https://scan.coverity.com/projects/dqm4hep-dqm4hep)
[![DOI](https://zenodo.org/badge/49281419.svg)](https://zenodo.org/badge/latestdoi/49281419)


<img src="icons/logo/png/dqm4hep-logo_dark.png"/> 

A Data Quality Monitoring for High Energy Physics

## Install

### Dependencies

* [ROOT](http://root.cern.ch) version >= 6.08
* [xdrstream](https://github.com/dqm4hep/xdrstream) version >= 1.1

### Install commands

```bash
source /path/to/root/bin/thisroot.sh
mkdir build
cd build
cmake -Dxdrstream_DIR=/path/to/xdrstream/lib/cmake [-DOPTIONS=...] ..
make install
```

where OPTIONS can be :
- DQM4hep_TESTING: Build unit testing binaries (default is ON)
- DQM4hep_WARNING_AS_ERROR: Whether to compile with -Werror (default is OFF)
- DQM4hep_EXTRA_WARNINGS: Whether to compile with -Wextra (default is ON)
- DQM4hep_DEV_WARNINGS: Additional developer warnings (default is OFF)

## Licensing

[![License](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl-3.0.en.html)

DQM4hep is distributed under the [GPLv3 License](http://www.gnu.org/licenses/gpl-3.0.en.html) 

## Bug report

Use the [github issues interface](https://github.Com/DQM4hep/DQM4hep/issues)
