#!/bin/bash

method=$1
flag=$2
value=$3

test_one_config() {


echo "rng_test_${method}_${co_num}_${flag}.log : "

cat rng_test_${method}_${co_num}_${flag}.log | grep -n  ${value}

}


co_num=1
test_one_config

co_num=4
test_one_config

co_num=16
test_one_config

co_num=64
test_one_config

co_num=256
test_one_config

co_num=1024
test_one_config

co_num=4096
test_one_config