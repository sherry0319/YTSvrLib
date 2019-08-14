#!/bin/bash
cd third_party
tar -xzf libevent-2.0.22-stable.tar.gz
cd libevent-2.0.22-stable
./configure && make install
cd ..
tar -xzf zlib-1.2.11.tar.gz
cd zlib-1.2.11
./configure && make install
cd ..
unzip libiconv-1.14.zip
cd libiconv-1.14
chmod 755 configure
./configure && make install
cd ..
unzip mysqlpp.zip
cd mysqlpp
chmod 755 configure
./configure && make install
cd ..
tar -xzf lz4.v1.8.3.tar.gz
cd lz4-1.8.3
make install
cd ../..