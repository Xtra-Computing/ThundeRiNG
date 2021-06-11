#!/bin/bash

TESTU01_PATH=/home/tanhs/graph_mining/TestU01

export LD_LIBRARY_PATH=${TESTU01_PATH}/lib:${LD_LIBRARY_PATH}
export LIBRARY_PATH=${TESTU01_PATH}/lib:${LIBRARY_PATH}


flag="b"
method=7
co_num=1


test_one_config() {

make -f prt.mk method=${method} co_num=${co_num}

mv rng_test.app pr_${co_num}.app

./pr_${co_num}.app | ./RNG_test stdin32

}



#kill  $(pidof rng_test*)


#co_num=1
test_one_config

