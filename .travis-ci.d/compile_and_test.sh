#!/bin/bash

ls -la

source dependencies/root/bin/thisroot.sh

git clone https://github.com/dqm4hep/dqm4hep.git
cd dqm4hep
mkdir build
cd build

cmake -DDQM4HEP_USE_MASTER=ON -DDQM4HEP_DOXYGEN_DOC=OFF -DDQM4HEP_TESTING=ON -DDQM4HEP_WARNING_AS_ERROR=ON -DDQM4HEP_DEV_WARNINGS=ON -DDQMCore_repository=$TRAVIS_BUILD_DIR -DDQMCore_version=$TRAVIS_COMMIT ..

if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

make install VERBOSE=1

if [ $? -ne 0 ]; then
    echo "Failed to run make"
    exit 1
fi

if [[ "${TRAVIS_OS_NAME}" == "linux" ]]; then export LDD_TOOL="ldd"; export LIB_EXT="so";
elif [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then export LDD_TOOL="otool -L"; export LIB_EXT="dylib";
fi

$LDD_TOOL ./lib/*.$LIB_EXT
$LDD_TOOL ./bin/*

