#!/bin/bash
base_dir=`pwd`
echo $base_dir
cd $base_dir"/cmake_release"
ls -l
make -j5 install

