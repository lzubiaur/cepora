@echo off

if not exist build\win (
    mkdir build\win
    )

pushd build\win

set PATH=.;C:\Users\lzubiaur\cmake-3.3.2\bin;%PATH% 

cmake.exe ^
    -G "Visual Studio 14 2015" ^
    -DBUILD_WIN=TRUE ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DCMAKE_BUILD_TYPE=Release ^
    ..\..

cmake.exe --build . --target install --config Release

popd
