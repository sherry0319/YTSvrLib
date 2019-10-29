#!/bin/bash
base_dir=`pwd`
echo $base_dir
svn update
cd $base_dir"/src"
autoscan && aclocal && autoheader && autoreconf -f -i -Wall,no-obsolete && automake && autoconf
chmod 755 configure
make distclean
mkdir -p linux_debug
mkdir -p linux_release
cd $base_dir"/src/linux_debug"
../configure
cd $base_dir"/src/linux_release"
../configure