#!/bin/sh
if ! [ -d "build/linux" ]; then 
    mkdir -p "build/linux"
fi
cd build/linux

export CXX="gcc"

msg=" --------------------- Build Succeeded ---------------------"

# Use default cmake
# CMAKE="cmake"
# Use custom cmake installation
CMAKE="/home/lzubiaur/dev/bin/cmake-3.3/bin/cmake"

$CMAKE \
    -DBUILD_LINUX=TRUE \
    -DCMAKE_BUILD_TYPE=Release \
    ../..

$CMAKE --build . --target install --config Release && echo $msg
