#!/bin/bash

# generate GL3W if needed
if ! [ -f "lib/gl3w/src/gl3w.c" ]; then
  pushd lib/gl3w
  ./gl3w_gen.py
  popd
fi

build='Release'
if [ "$1" == "debug" ]; then
  build='Debug'
fi

if ! [ -d "build/linux" ]; then
  mkdir -p "build/linux"
fi
cd build/linux

# C and C++ compiler
export CC="gcc"
export CXX="g++"

msg=" --------------------- Build Succeeded ---------------------"

# Use default cmake
CMAKE="cmake"
# Use custom cmake installation
# CMAKE="/home/lzubiaur/dev/bin/cmake-3.3/bin/cmake"

$CMAKE \
    -DBUILD_LINUX=TRUE \
    -DCMAKE_BUILD_TYPE=$build \
    ../..

$CMAKE --build . --target install --config $build && echo $msg
