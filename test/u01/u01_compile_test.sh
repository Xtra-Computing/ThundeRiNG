#!/bin/bash

TESTU01_PATH=/home/tanhs/graph_mining/TestU01

export LD_LIBRARY_PATH=${TESTU01_PATH}/lib:${LD_LIBRARY_PATH}
export LIBRARY_PATH=${TESTU01_PATH}/lib:${LIBRARY_PATH}


flag="m"
method=12
co_num=1024


test_one_config() {

make -f rng_test.mk method=${method} co_num=${co_num}

./rng_test.app


}


test_one_config