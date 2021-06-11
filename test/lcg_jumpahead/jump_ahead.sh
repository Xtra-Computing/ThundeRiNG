#!/bin/bash

rm -rf jump_ahead_test.app

gcc lcg_jump_ahead_test.c ../pcg_basic/pcg_basic.c -I../ -o jump_ahead_test.app

./jump_ahead_test.app