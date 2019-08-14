#!/bin/bash
cd third_party
tar -xzf curl-7.53.1.tar.gz
cd curl-7.53.1
./configure && make install
cd ..
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
./configure && make install
cd ..
unzip mysqlpp.zip
cd mysqlpp
chmod 755 configure
./configure && make install
cd ../..