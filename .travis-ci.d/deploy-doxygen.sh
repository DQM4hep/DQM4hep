#!/bin/bash

doxygenDirectory=""
repository=$(basename $TRAVIS_REPO_SLUG | tr '[:upper:]' '[:lower:]')
username=$(dirname $TRAVIS_REPO_SLUG | tr '[:upper:]' '[:lower:]')

if [ ! "${CXX}" = "g++" ] || [ ! "${TRAVIS_OS_NAME}" = "linux" ]
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
elif [ ! -z "$TRAVIS_TAG" ]
then # pushed a tag
  doxygenDirectory="${repository}/$TRAVIS_TAG"
else
  echo "Not on a valid branch for pushing doxygen !"
  exit 0
fi

cd "$TRAVIS_BUILD_DIR"

source dependencies/root/bin/thisroot.sh

cd build

# activate doxygen building target
cmake -DINSTALL_DOC=ON ..

if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

# build doxygen documentation
make doc

if [ $? -ne 0 ]; then
    echo "Failed to run make doc"
    exit 1
fi

# get the doxygen package
cd ../doc
git clone https://rete:$GITHUB_ACCESS_TOKEN@github.com/dqm4hep/dqm4hep-doxygen.git --branch=gh-pages
cd dqm4hep-doxygen

# create the documentation directory
mkdir -p doxygen/${doxygenDirectory}
cd doxygen/${doxygenDirectory}

# remove current documentation if exists
rm -rf *

# copy the new one in place
cp -r $TRAVIS_BUILD_DIR/build/docbuild/html/* .

# commit the new doc
git add ./*
git -c user.name='travis' -c user.email='travis' commit -m "Updated ${doxygenDirectory} doxygen documentation, build commit: $TRAVIS_COMMIT"
git remote rm origin
git remote add origin https://rete:$GITHUB_ACCESS_TOKEN@github.com/dqm4hep/dqm4hep-doxygen.git
git push -fq origin gh-pages &2>/dev/null
