#!/bin/bash

ls -la

mkdir -p build
cd build

cmake -DDQM4HEP_DOXYGEN_DOC=OFF -DCMAKE_MODULE_PATH=$PWD/../dependencies/dqm4hep/cmake -Dxdrstream_DIR=$PWD/../dependencies/xdrstream -DDQMCore_DIR=$PWD/../dependencies/dqm4hep-core -DDQM4HEP_TESTING=ON -DDQM4HEP_WARNING_AS_ERROR=ON -DDQM4HEP_DEV_WARNINGS=ON ..

if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

make install VERBOSE=1

if [ $? -ne 0 ]; then
    echo "Failed to run make"
    exit 1
fi

# ctest -V
#
# if [ $? -ne 0 ]; then
#     echo "Failed to run cmake tests"
#     exit 1
# fi
