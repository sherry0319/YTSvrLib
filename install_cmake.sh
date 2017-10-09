#!/bin/bash
base_dir=`pwd`
rm -rf /usr/local/lib/libytsvrlib.so
cp -rf $base_dir"/libytsvrlib.so" /usr/local/lib/libytsvrlib.so
rm -rf /usr/lib/libytsvrlib.so
ln /usr/local/lib/libytsvrlib.so /usr/lib/
mkdir -p /usr/include/ytsvrlib
cp -rfuv $base_dir/../src/*.h $base_dir/../Header/
cp -rfuv $base_dir/../Header/* /usr/include/ytsvrlib/
ldconfig
