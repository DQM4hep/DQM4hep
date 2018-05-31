#!/bin/bash

ls -la

source dependencies/root/bin/thisroot.sh
export CURRENT_COMMIT_ID=`git rev-parse HEAD`

git clone https://github.com/dqm4hep/dqm4hep.git
cd dqm4hep
mkdir build
cd build

cmake -DDQM4HEP_USE_MASTER=ON -DDQM4HEP_DOXYGEN_DOC=OFF -DDQM4HEP_TESTING=ON -DDQM4HEP_WARNING_AS_ERROR=ON -DDQM4HEP_DEV_WARNINGS=ON -DDQMCore_repository=$PWD/../.. -DDQMCore_version=$CURRENT_COMMIT_ID ..

if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

make install VERBOSE=1

if [ $? -ne 0 ]; then
    echo "Failed to run make"
    exit 1
fi
