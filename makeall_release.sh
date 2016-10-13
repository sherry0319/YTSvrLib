#!/bin/bash
cd src
make -j5 install CPPFLAGS="-O2"
cd ..
