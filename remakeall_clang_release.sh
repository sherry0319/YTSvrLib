#!/bin/bash
base_dir=`pwd`
echo $base_dir
svn update
cd $base_dir"/cmake_release"
make clean && make -j5 install
cd $base_dir