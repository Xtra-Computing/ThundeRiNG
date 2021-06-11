#!/bin/bash

TESTU01_PATH=/home/tanhs/graph_mining/TestU01

export LD_LIBRARY_PATH=${TESTU01_PATH}/lib:${LD_LIBRARY_PATH}
export LIBRARY_PATH=${TESTU01_PATH}/lib:${LIBRARY_PATH}


flag="b"
method=17
co_num=1


test_one_config() {

	make -f rng_test.mk method=${method} co_num=${co_num}

	mv rng_test.app rng_test_${co_num}.app
	nohup ./rng_test_${co_num}.app -v -${flag} 2>&1 | tee rng_test_${method}_${co_num}_${flag}.log &

}

#kill  $(pidof rng_test*)



all_test=1

if [ ${all_test} -eq 1 ];
	    then
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

	    else

		    co_num=4096
		    test_one_config

	    fi




