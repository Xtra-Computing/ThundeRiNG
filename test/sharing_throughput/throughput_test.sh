#!/bin/bash

g++ -g -O3 throughput_test.cpp  -o throughput_test.app


readelf -a  throughput_test.app  > throughput_test_readelf.log

objdump -d  throughput_test.app   > throughput_test_objdump.log
