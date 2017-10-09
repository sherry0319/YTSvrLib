#!/bin/bash
base_dir=`pwd`
echo $base_dir
rm -rf cmake_debug
rm -rf cmake_release
mkdir -p cmake_debug
mkdir -p cmake_release
cd $base_dir"/cmake_debug"
cmake -DWITH_DEBUG=ON -DCMAKE_TOOLCHAIN_FILE=../linux.clang.toolchain.cmake ..
cd $base_dir"/cmake_release"
cmake -DCMAKE_TOOLCHAIN_FILE=../linux.clang.toolchain.cmake ..
