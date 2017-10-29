#!/bin/bash

ls -la
mkdir -p dependencies && cd dependencies

# install dqm4hep cmake macros
git clone https://github.com/dqm4hep/dqm4hep.git

ls -la
cd ..
