################################################################################
# Cmake script for cvmfs installation 
################################################################################

SET( LCG_INSTALL "/cvmfs/sft.cern.ch/lcg/releases/LCG_84" CACHE PATH "Path to LCG installation" FORCE)
MARK_AS_ADVANCED( LCG_INSTALL )

# Ensure proper version of Qt will be used
SET (ENV{QTDIR} ${LCG_INSTALL}/qt/4.8.4/x86_64-slc6-gcc48-opt)
SET( CMAKE_PREFIX_PATH 
     ${LCG_INSTALL}/Boost/1.59.0_python2.7/x86_64-slc6-gcc48-opt;
     ${LCG_INSTALL}/qt/4.8.4/x86_64-slc6-gcc48-opt;
     ${LCG_INSTALL}/ROOT/6.06.02/x86_64-slc6-gcc48-opt;
     ${CMAKE_PREFIX_PATH}
CACHE PATH "CMAKE_PREFIX_PATH" FORCE )

option(Boost_NO_BOOST_CMAKE "dont use cmake find module for boost" ON)
