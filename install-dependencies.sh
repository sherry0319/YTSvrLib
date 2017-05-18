#!/bin/bash
lib_install_base_dir="/usr/local/"
cd third_party
tar -xzf curl-7.53.1.tar.gz
cd curl-7.53.1
./configure --prefix=$lib_install_base_dir"curl" && make -j5 && sudo make install
sudo ln -s $lib_install_base_dir"curl/lib/libcurl.so" /usr/lib/libcurl.so
cd ..
tar -xzf libevent-2.0.22-stable.tar.gz
cd libevent-2.0.22-stable
./configure --prefix=$lib_install_base_dir"/libevent2" && make -j5 && sudo make install
sudo ln -s $lib_install_base_dir"libevent2/lib/libevent.so" /usr/lib/libevent.so
sudo ln -s $lib_install_base_dir"libevent2/lib/libevent_pthreads.so" "/usr/lib/libevent_pthreads.so"
sudo ln -s $lib_install_base_dir"libevent2/lib/libevent_openssl.so" "/usr/lib/libevent_openssl.so"
sudo ln -s $lib_install_base_dir"libevent2/lib/libevent_extra.so" "/usr/lib/libevent_extra.so"
sudo ln -s $lib_install_base_dir"libevent2/lib/libevent_core.so" "/usr/lib/libevent_core.so"
cd ..
tar -xzf zlib-1.2.11.tar.gz
cd zlib-1.2.11
./configure --prefix=$lib_install_base_dir"zlib" && make -j5 && sudo make install
sudo ln -s $lib_install_base_dir"zlib/lib/libz.so" "/usr/lib/libz.so"
cd ..
unzip libiconv-1.14.zip
cd libiconv-1.14
./configure --prefix=$lib_install_base_dir"iconv" && make -j5 && sudo make install
sudo ln -s $lib_install_base_dir"iconv/lib/libiconv.so" "/usr/lib/libiconv.so"
cd ..
unzip mysqlpp.zip
cd mysqlpp
chmod 755 configure
./configure --prefix=$lib_install_base_dir"mysqlpp" && make -j5 && sudo make install
sudo ln -s $lib_install_base_dir"mysqlpp/lib/libmysqlpp.so" "/usr/lib/libmysqlpp.so"
cd ..
tar -xzf gperftools-2.5.tar.gz
cd gperftools-2.5
./configure --prefix=$lib_install_base_dir"tcmalloc" --enable-frame-pointers --enable-minimal && make -j5 && sudo make install
sudo ln -s $lib_install_base_dir"tcmalloc/lib/libtcmalloc_minimal.so" "/usr/lib/libtcmalloc_minimal.so"
cd ..