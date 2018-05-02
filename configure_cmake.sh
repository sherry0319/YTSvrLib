#!/bin/bash
base_dir=`pwd`
echo $base_dir
svn update
rm -rf cmake_debug
rm -rf cmake_release
mkdir -p cmake_debug
mkdir -p cmake_release
cd $base_dir"/cmake_debug"
cmake -DWITH_DEBUG=ON ..
cd $base_dir"/cmake_release"
cmake ..
