#!/bin/bash



TESTU01_PATH=/home/tanhs/graph_mining/TestU01

export LD_LIBRARY_PATH=${TESTU01_PATH}/lib:${LD_LIBRARY_PATH}
export LIBRARY_PATH=${TESTU01_PATH}/lib:${LIBRARY_PATH}


flag="m"
method=17
co_num=1000


test_one_config() {

make -f hwd/hwd_test.mk method=${method} co_num=${co_num}

mv hwd_test.app hwd_test_${co_num}.app
./hwd_test_${co_num}.app  2>&1 | tee hwd_test_${method}_${co_num}_${flag}.log &

}

test_one_config
