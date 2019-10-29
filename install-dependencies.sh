#!/bin/bash
base_dir=`pwd`"/third_party"
cd $base_dir
tar -xzf curl-7.53.1.tar.gz
cd curl-7.53.1
./configure && sudo make install
cd $base_dir
tar -xzf zlib-1.2.11.tar.gz
cd zlib-1.2.11
./configure && sudo make install
cd $base_dir
unzip libiconv-1.14.zip
cd libiconv-1.14
sudo chmod 755 configure
./configure && sudo make install
cd $base_dir
sudo ls -l /usr/lib/mysql
unzip mysqlpp.zip
cd mysqlpp
chmod 755 configure
./configure --with-mysql-lib=/usr/lib/x86_64-linux-gnu && sudo make install
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