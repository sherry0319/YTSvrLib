#!/bin/bash
base_dir=`pwd`
echo $base_dir
svn update
cd $base_dir"/src/linux_debug"
make -j5 install CPPFLAGS="-g -DDEBUG64 -O0" BASEDIR=$base_dir"/src"
cd $base_dir
