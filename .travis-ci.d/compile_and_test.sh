#!/bin/bash

ls -la

source dependencies/root/bin/thisroot.sh

mkdir -p build
cd build

cmake -DDQM4HEP_DOXYGEN_DOC=OFF -Dxdrstream_DIR=$PWD/../dependencies/xdrstream -DCMAKE_MODULE_PATH=$PWD/../dependencies/dqm4hep/cmake -DDQM4HEP_TESTING=ON -DDQM4HEP_WARNING_AS_ERROR=ON -DDQM4HEP_DEV_WARNINGS=ON ..

if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

make install VERBOSE=1

if [ $? -ne 0 ]; then
    echo "Failed to run make"
    exit 1
fi

ldd ./lib/*.so
ldd ./bin/*

ctest -V

if [ $? -ne 0 ]; then
    echo "Failed to run cmake tests"
    exit 1
fi
