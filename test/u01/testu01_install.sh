#!/bin/bash
mkdir TestU01
cd TestU01
basedir=`pwd`
curl -OL http://simul.iro.umontreal.ca/testu01/TestU01.zip
unzip -q TestU01.zip
cd TestU01-1.2.3
./configure --prefix="$basedir"
make -j 6
make -j 6 install
cd ..
    mkdir lib-so
    mv lib/*.so lib-so/.