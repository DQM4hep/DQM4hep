#!/bin/bash

if [ -z ${COVERITY_SCAN_TOKEN} ]
then
  echo "No coverity scan token set !"
  exit 1
fi

export description=`date`
export COVERITY_REPO=`echo ${TRAVIS_REPO_SLUG} | sed 's/\//\%2F/g'`

# install coverity scan
wget https://scan.coverity.com/download/linux64 --post-data "token=${COVERITY_SCAN_TOKEN}&project=${COVERITY_REPO}" -O coverity_tool.tgz
mkdir cov-analysis-linux64 
tar -xf coverity_tool.tgz -C cov-analysis-linux64 --strip-components=2
export PATH=$PWD/cov-analysis-linux64/bin:$PATH

# run cmake and build with coverity
source dependencies/root/bin/thisroot.sh
mkdir -p build && cd build
cmake -DINSTALL_DOC=OFF -Dxdrstream_DIR=$PWD/../dependencies/xdrstream -DCMAKE_MODULE_PATH=$PWD/../dependencies/dqm4hep/cmake -DBUILD_TESTS=ON ..
cov-configure --comptype gcc --compiler /usr/bin/${CC}
cov-configure --comptype g++ --compiler /usr/bin/${CXX}
cov-build --dir cov-int make -j2
tar czvf myproject.tgz cov-int

# post coverity scan report
curl --form token=${COVERITY_SCAN_TOKEN} --form email=dqm4hep@gmail.com --form file=@myproject.tgz --form version="master" --form description="${description}" https://scan.coverity.com/builds?project=${COVERITY_REPO}

