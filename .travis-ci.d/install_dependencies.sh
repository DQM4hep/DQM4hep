#!/bin/bash

ls -la
mkdir -p dependencies && cd dependencies

# install dqm4hep cmake macros
git clone https://github.com/dqm4hep/dqm4hep.git

# install jsoncpp
git clone https://github.com/open-source-parsers/jsoncpp.git
cd jsoncpp
mkdir install
export JSONCPP_INSTALL_DIR=$PWD/install
git checkout 1.8.3
mkdir -p build && cd build
cmake -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_WARNING_AS_ERROR=OFF -DCMAKE_INSTALL_PREFIX=${JSONCPP_INSTALL_DIR} ..

if [ $? -ne 0 ]; then
    echo "Failed to run jsoncpp cmake"
    exit 1
fi

make install VERBOSE=1

if [ $? -ne 0 ]; then
    echo "Failed to run json make"
    exit 1
fi

cd ..
ls -lr install

cd ..
