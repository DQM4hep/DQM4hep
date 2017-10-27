#!/bin/bash

ls -la
mkdir -p build
cd build
cmake -DINSTALL_DOC=OFF -Dxdrstream_DIR=$XDRSTREAM_DIR -DCMAKE_MODULE_PATH=$DQM4HEP_DIR/cmake ..
make install -j 4
ctest -V
