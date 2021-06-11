#!/bin/bash

TESTU01_PATH=/home/tanhs/graph_mining/TestU01

export LD_LIBRARY_PATH=${TESTU01_PATH}/lib:${LD_LIBRARY_PATH}
export LIBRARY_PATH=${TESTU01_PATH}/lib:${LIBRARY_PATH}


flag="s"
method=10
co_num=1


test_one_config() {

make -f rng_test.mk method=${method} co_num=${co_num}

mv rng_test.app rng_test_${co_num}.app
./rng_test_${co_num}.app -v -${flag} 2>&1 | tee rng_test_lcg_de_${method}_${co_num}_${flag}.log &

}

#kill  $(pidof rng_test*)



all_test=1


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

#co_num=16384
#test_one_config


wait


flag="m"


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

#co_num=16384
#test_one_config


wait


flag="b"


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

#co_num=16384
#test_one_config

wait
