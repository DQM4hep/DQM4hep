#!/bin/bash

ls -la

source dependencies/root/bin/thisroot.sh

git clone https://github.com/dqm4hep/dqm4hep.git
cd dqm4hep
mkdir build
cd build

# Disable autamatic testing within make on osx. See comment below
if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then export DQM4HEP_TESTING="OFF";
else export DQM4HEP_TESTING="ON";
fi

cmake -DDQM4HEP_USE_MASTER=ON -DDQM4HEP_DOXYGEN_DOC=OFF -DDQM4HEP_TESTING=$DQM4HEP_TESTING -DDQM4HEP_WARNING_AS_ERROR=ON -DDQM4HEP_DEV_WARNINGS=ON -DDQMOnline_repository=$TRAVIS_BUILD_DIR -DDQMOnline_version=$TRAVIS_COMMIT ..

if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

make install VERBOSE=1

if [ $? -ne 0 ]; then
    echo "Failed to run make"
    exit 1
fi

# For some reason, running ctest from within make leads to an instantaneous Child aborted exception being thrown on osx. Running the tests manually works without trouble though...
if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
    cd DQMOnline-prefix/src/DQMOnline-build/
    ctest -V

    if [ $? -ne 0 ]; then
        echo "Failed to run cmake tests"
        exit 1
    fi
    cd -
fi
