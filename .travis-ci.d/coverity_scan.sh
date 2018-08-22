#!/bin/bash

if [ -z ${COVERITY_SCAN_TOKEN} ]
then
  echo "No coverity scan token set !"
  exit 1
fi

export description=`date`
export COVERITY_REPO="DQM4HEP%2Fdqm4hep"

# install coverity scan
wget https://scan.coverity.com/download/Linux --post-data "token=${COVERITY_SCAN_TOKEN}&project=DQM4HEP%2Fdqm4hep" -O coverity_tool.tgz
mkdir cov-analysis-Linux
tar -xf coverity_tool.tgz -C cov-analysis-Linux --strip-components=2 &> /dev/null
export PATH=$PWD/cov-analysis-Linux/bin:$PATH

# run cmake and build with coverity
source dependencies/root/bin/thisroot.sh
mkdir -p build && cd build

cmake \
  -DDQM4hep_DOXYGEN_DOC=OFF \
  -DDQM4hep_TESTING=ON \
  -DDQM4hep_WARNING_AS_ERROR=ON \
  -DDQM4hep_DEV_WARNINGS=ON \
  ..

cov-build --dir cov-int make install
tar czvf myproject.tgz cov-int

# post coverity scan report
curl --form token=${COVERITY_SCAN_TOKEN} --form email=dqm4hep@gmail.com --form file=@myproject.tgz --form version="master" --form description="${description}" https://scan.coverity.com/builds?project=${COVERITY_REPO}

