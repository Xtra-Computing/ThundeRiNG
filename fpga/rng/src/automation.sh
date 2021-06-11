#!/bin/bash

ulimit -n 4096
NCU=8

rm -rf ./gen_link.app
make  -f ./src/automation/auto_gen_code.mk CUs=${NCU}  code_gen.app
gcc ./src/automation/template/gen_link.c -DCUs=${NCU} -o gen_link.app


mkdir -p ./src/sou
rm -rf ./src/sou/rng_instance*
./code_gen.app ./src/sou/template/template_rng_instance.cpp    src/sou/rng_instance
./gen_link.app     > krnl_incr.cfg