#!/bin/bash
rm -rf /usr/local/lib/libytsvrlib.so
cp -rf libytsvrlib.so /usr/local/lib/libytsvrlib.so
rm -rf /usr/lib/libytsvrlib.so
ln /usr/local/lib/libytsvrlib.so /usr/lib/
mkdir -p /usr/include/ytsvrlib
cp -rfuv ../*.h ../../Header/
cp -rfuv ../../Header/* /usr/include/ytsvrlib/
