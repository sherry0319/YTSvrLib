#!/bin/bash
base_dir=`pwd`
cd third_party
tar -xzf curl-7.53.1.tar.gz
cd curl-7.53.1
./configure && make install
cd $base_dir
tar -xzf zlib-1.2.11.tar.gz
cd zlib-1.2.11
./configure && make install
cd $base_dir
unzip libiconv-1.14.zip
cd libiconv-1.14
./configure && make install
cd $base_dir
unzip mysqlpp.zip
cd mysqlpp
chmod 755 configure
./configure && make install
cd $base_dir
unzip cpp_redis.zip
mkdir -p build
cd build
cmake ..
make install
cd $base_dir
tar -xzf lz4.v1.8.3.tar.gz
cd lz4-1.8.3
make install
cd ../..