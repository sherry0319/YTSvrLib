#!/bin/bash
base_dir=`pwd`
echo $base_dir
cd $base_dir"/cmake_debug"
make clean && make -j5 install
cd $base_dir
