# DQM4HEP ( Data Quality Monitoring for High Energy Physics )
IPNL/CNRS

## INSTALL:

Can be built standalonly or using ilcsoft (http://ilcsoft.desy.de/portal)

### Standalone install

In the root directory :

```bash
  mkdir build
  cd build
  cmake -C $ILCSOFT/ILCSoft.cmake ..
  make
```

On MAC-OSX
if you encounter this error when compiling did: 
	"./src/did/did.h:1:10: fatal error: 'Mrm/MrmAppl.h' file not found"
Check if OpenMotif is installed in /usr/OpenMotif
If it’s not change MOTIFINC and MOTIFLIB variables in makefile_did accordingly
Eg if using macport:
	MOTIFINC = /opt/local/include
	MOTIFLIB = /opt/local/lib

BUG REPORT:
===========

Report bugs to :
<dqm4hep@gmail.com>
