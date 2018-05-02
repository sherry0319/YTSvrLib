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
wget https://dev.mysql.com/get/Downloads/Connector-C/mysql-connector-c-6.1.11-src.zip
unzip mysql-connector-c-6.1.11-src.zip
cd mysql-connector-c-6.1.11-src
mkdir -p build
cd build
cmake ..
make install
cd ..
cd ..
unzip mysqlpp.zip
cd mysqlpp
chmod 755 configure
./configure && make install
cd ..
tar -xzf libwebsockets-2.4.2.tar.gz
cd libwebsockets-2.4.2
mkdir -p build
cd build
cmake .. -DLWS_WITH_HTTP2=ON -DLWS_WITH_LIBEVENT=ON -DLWS_WITH_LIBUV=ON -DCMAKE_BUILD_TYPE=RELEASE -DLWS_WITH_NO_LOGS=ON
make install
cd ../..