
co_num:= 2
method:= 1
SELECTED_METHOD=${method}
CO_NUM=${co_num}


TESTU01_PATH=/home/tanhs/graph_mining/TestU01
CPP_CFLAGS= -I ${TESTU01_PATH}/include -I ../ -Iinclude
CPP_CFLAGS+= -DSELECTED_METHOD=${SELECTED_METHOD}
CPP_CFLAGS+= -DCO_NUM=${CO_NUM}
CPP_CFLAGS+= -I ./pcg_basic/ -I .

CPP_LDFLAGS= -L ${TESTU01_PATH}/lib -ltestu01 -lprobdist -lmylib -lm

SRC=./hwd/hwd.c
SRC+=./pcg_basic/pcg_basic.c

CPP_COMPILER=g++
#CPP_COMPILER=clang++-11 -v

all:
	rm -f ./rng_test.app
	${CPP_COMPILER}  -std=c++14 -Wall -O3 -o hwd_test.app ${SRC} ${CPP_CFLAGS}  ${CPP_LDFLAGS} -lm