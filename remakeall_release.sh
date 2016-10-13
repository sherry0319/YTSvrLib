#!/bin/bash
cd src
make clean
make -j5 install CPPFLAGS="-O2"
cd ..