########################################
# README file for DQM4HEP package
# @author Eté Rémi
# @version 1.0.0 17/02/2014
# @copyright IPNL, CNRS
########################################


//DQM4HEP ( Data Quality Monitoring for High Energy Physics )


INSTALL:
=========

Needs ilcsoft installed (http://ilcsoft.desy.de/portal). Tested with the following version :
 * v01-17-03
 * v01-17-08

To install the package : 

  $ mkdir build
  $ cd build
  $ cmake -C $ILCSOFT/ILCSoft.cmake ..
  $ make

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
