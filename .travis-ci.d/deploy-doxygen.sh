#!/bin/bash

doxygenDirectory=""
pkgVersion=""
repository=$(basename $TRAVIS_REPO_SLUG | tr '[:upper:]' '[:lower:]')
username=$(dirname $TRAVIS_REPO_SLUG | tr '[:upper:]' '[:lower:]')

if [ ! "${CXX}" = "g++" ] || [ ! "${TRAVIS_OS_NAME}" = "linux" ] || [ "${TRAVIS_EVENT_TYPE}" = "cron" ]
then
  echo "Build and push doxygen only one per push"
  exit 0
fi

# check correct user name
if [ ! "${username}" = "dqm4hep" ]
then
  echo "Invalid username for pushing doxygen !"
  echo "Username: ${username}, repository: ${repository}"
  exit 0
fi

# check if PR
if [ ! "$TRAVIS_PULL_REQUEST" = "false" ]
then
  echo "Doxygen not deployed on pull request !"
  exit 0
fi

if [ "$TRAVIS_BRANCH" = "master" ]
then # master branch case
  doxygenDirectory="${repository}/master"
  pkgVersion="master"
elif [ ! -z "$TRAVIS_TAG" ]
then # pushed a tag
  pkgVersion="$TRAVIS_TAG"
  doxygenDirectory="${repository}/$TRAVIS_TAG"
else
  echo "Not on a valid branch for pushing doxygen !"
  exit 0
fi

cd "$TRAVIS_BUILD_DIR"

source dependencies/root/bin/thisroot.sh

cd dqm4hep/build

# activate doxygen building target
cmake -DDQM4HEP_DOXYGEN_DOC=ON ..

if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

# build doxygen documentation
make install

if [ $? -ne 0 ]; then
    echo "Failed to run make doc"
    exit 1
fi

# get the doxygen package
git clone https://rete:$GITHUB_ACCESS_TOKEN@github.com/dqm4hep/dqm4hep-doxygen.git --branch=gh-pages
cd dqm4hep-doxygen

# add (if not exists) the package version to the web page
python doxygen.py --input meta.json --output meta.json --add-pkg --pkg-name ${repository} --pkg-version ${pkgVersion}
git add meta.json 

# create the documentation directory
mkdir -p doxygen/${doxygenDirectory}
cd doxygen/${doxygenDirectory}

# remove current documentation if exists
rm -rf *

# copy the new one in place
cp -r $TRAVIS_BUILD_DIR/dqm4hep/doc/DQMNet/html/* .

# commit the new doc
git add ./*
git -c user.name='travis' -c user.email='travis' commit -m "Updated ${doxygenDirectory} doxygen documentation, build commit: $TRAVIS_COMMIT"
git remote rm origin
git remote add origin https://rete:$GITHUB_ACCESS_TOKEN@github.com/dqm4hep/dqm4hep-doxygen.git
git push -fq origin gh-pages >/dev/null 2>&1
