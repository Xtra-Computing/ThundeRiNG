#!/bin/bash
gcc jumpxor32.c -o genxor32_state.app

./genxor32_state.app | tee xor32s.h