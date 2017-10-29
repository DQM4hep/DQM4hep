#!/bin/bash

ls -la

source dependencies/root/bin/thisroot.sh

mkdir -p build
cd build

cmake -DINSTALL_DOC=OFF -Dxdrstream_DIR=$PWD/../dependencies/xdrstream -DJSONCPP_DIR=$PWD/../dependencies/jsoncpp/install -DCMAKE_MODULE_PATH=$PWD/../dependencies/dqm4hep/cmake -DBUILD_TESTS=ON ..

if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

make install VERBOSE=1

if [ $? -ne 0 ]; then
    echo "Failed to run make"
    exit 1
fi

ctest -V

if [ $? -ne 0 ]; then
    echo "Failed to run cmake tests"
    exit 1
fi
