#!/bin/bash

ls -la
mkdir -p dependencies && cd dependencies

# install root
wget https://root.cern.ch/download/root_v6.10.08.Linux-ubuntu14-x86_64-gcc4.8.tar.gz
tar -xf root_v6.10.08.Linux-ubuntu14-x86_64-gcc4.8.tar.gz
source root/bin/thisroot.sh
root-config --version
cd ..

# # install dqm4hep cmake macros
# git clone https://github.com/dqm4hep/dqm4hep.git
# export DQM4HEP_DIR=$PWD/dqm4hep
# 
# # install xdrstream
# git clone https://github.com/dqm4hep/xdrstream.git
# cd xdrstream
# export XDRSTREAM_DIR=$PWD
# mkdir build && cd build
# cmake -DINSTALL_DOC=OFF ..
# 
# if [ $? -ne 0 ]; then
#     echo "Failed to run xdrstream cmake"
#     exit 1
# fi
# 
# make install VERBOSE=1
# 
# if [ $? -ne 0 ]; then
#     echo "Failed to run xdrstream make"
#     exit 1
# fi
# 
# cd ../..
# 
# ls -la
# cd ..
