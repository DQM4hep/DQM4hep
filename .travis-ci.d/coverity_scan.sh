#!/bin/bash

if [ -z ${COVERITY_SCAN_TOKEN} ]
then
  echo "No coverity scan token set !"
  exit 1
fi

export description=`date`
export COVERITY_REPO=`echo ${TRAVIS_REPO_SLUG} | sed 's/\//\%2F/g'`

# install coverity scan
wget https://scan.coverity.com/download/Linux --post-data "token=${COVERITY_SCAN_TOKEN}&project=${COVERITY_REPO}" -O coverity_tool.tgz
mkdir cov-analysis-Linux
tar -xf coverity_tool.tgz -C cov-analysis-Linux --strip-components=2 &> /dev/null
export PATH=$PWD/cov-analysis-Linux/bin:$PATH

# run cmake and build with coverity
source dependencies/root/bin/thisroot.sh
mkdir -p build && cd build
cmake -DDQM4HEP_DOXYGEN_DOC=OFF -DCMAKE_MODULE_PATH=$PWD/../dependencies/dqm4hep/cmake -Dxdrstream_DIR=$PWD/../dependencies/xdrstream -DDQMCore_DIR=$PWD/../dependencies/dqm4hep-core -DDQM4HEP_TESTING=ON -DDQM4HEP_WARNING_AS_ERROR=ON -DDQM4HEP_DEV_WARNINGS=ON ..
cov-build --dir cov-int make -j2
tar czvf myproject.tgz cov-int

# post coverity scan report
curl --form token=${COVERITY_SCAN_TOKEN} --form email=dqm4hep@gmail.com --form file=@myproject.tgz --form version="master" --form description="${description}" https://scan.coverity.com/builds?project=${COVERITY_REPO}

