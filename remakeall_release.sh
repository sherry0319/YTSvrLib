#!/bin/bash
base_dir=`pwd`
echo $base_dir
cd $base_dir"/src/linux_release"
sudo make clean
sudo make -j5 install CPPFLAGS="-O2" BASEDIR=$base_dir"/src"
cd $base_dir