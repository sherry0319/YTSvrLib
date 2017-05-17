#!/bin/bash
cd third_party
tar -xzf curl-7.53.1.tar.gz
cd curl-7.53.1
./configure --prefix=/usr/local/curl && make -j5 && make install
ln -s /usr/local/curl/lib/libcurl.so /usr/lib/libcurl.so
cd ..
tar -xzf libevent-2.0.22-stable.tar.gz
cd libevent-2.0.22-stable
./configure --prefix=/usr/local/libevent2 && make -j5 && make install
ln -s /usr/local/libevent2/lib/libevent.so /usr/lib/libevent.so
ln -s /usr/local/libevent2/lib/libevent_pthreads.so /usr/lib/libevent_pthreads.so
ln -s /usr/local/libevent2/lib/libevent_openssl.so /usr/lib/libevent_openssl.so
ln -s /usr/local/libevent2/lib/libevent_extra.so /usr/lib/libevent_extra.so
ln -s /usr/local/libevent2/lib/libevent_core.so /usr/lib/libevent_core.so
cd ..
tar -xzf zlib-1.2.11.tar.gz
cd zlib-1.2.11
./configure --prefix=/usr/local/zlib && make -j5 && make install
ln -s /usr/local/zlib/lib/libz.so /usr/lib/libz.so
cd ..
unzip libiconv-1.14.zip
cd libiconv-1.14
./configure --prefix=/usr/local/iconv && make -j5 && make install
ln -s /usr/local/iconv/lib/libiconv.so /usr/lib/libiconv.so
cd ..
unzip mysqlpp.zip
cd mysqlpp
chmod 755 configure
./configure --prefix=/usr/local/mysqlpp && make -j5 && make install
ln -s /usr/local/mysqlpp/lib/libmysqlpp.so /usr/lib/libmysqlpp.so
cd ..
