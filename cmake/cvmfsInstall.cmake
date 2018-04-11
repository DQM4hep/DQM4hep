################################################################################
# Cmake script for cvmfs installation 
################################################################################

set( LCG_INSTALL "/cvmfs/sft.cern.ch/lcg/releases/LCG_84" CACHE PATH "Path to LCG installation" FORCE)
mark_as_advanced( LCG_INSTALL )

# Ensure proper version of Qt will be used
set( ENV{QTDIR} ${LCG_INSTALL}/qt/4.8.4/x86_64-slc6-gcc48-opt )
set( CMAKE_PREFIX_PATH 
     ${LCG_INSTALL}/Boost/1.59.0_python2.7/x86_64-slc6-gcc48-opt;
     ${LCG_INSTALL}/qt/4.8.4/x86_64-slc6-gcc48-opt;
     ${LCG_INSTALL}/ROOT/6.06.02/x86_64-slc6-gcc48-opt;
     ${CMAKE_PREFIX_PATH}
CACHE PATH "CMAKE_PREFIX_PATH" FORCE )

option( Boost_NO_BOOST_CMAKE "dont use cmake find module for boost" ON )
