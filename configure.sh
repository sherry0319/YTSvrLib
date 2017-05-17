#!/bin/bash
base_dir=`pwd`
echo $base_dir
cd $base_dir"/src"
chmod 755 configure
mkdir -p linux_debug
mkdir -p linux_release
cd $base_dir"/src/linux_debug"
../configure
cd $base_dir"/src/linux_release"
../configure