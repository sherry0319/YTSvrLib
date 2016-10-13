#!/bin/bash
cd src
make -j5 install CPPFLAGS="-g -DDEBUG64 -O0"
cd ..
