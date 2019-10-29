#!/bin/bash
base_dir=`pwd`"/third_party"
cd $base_dir
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
sudo chmod 755 configure
./configure && sudo make install
cd $base_dir
sudo ll /usr/lib64/ | grep mysql
sudo ll /usr/lib/ | grep mysql
unzip mysqlpp.zip
cd mysqlpp
chmod 755 configure
./configure && make install
cd $base_dir
unzip cpp_redis.zip
mkdir -p build
cd build
cmake ..
sudo make install
cd $base_dir
tar -xzf lz4.v1.8.3.tar.gz
cd lz4-1.8.3
sudo make install
cd $base_dir