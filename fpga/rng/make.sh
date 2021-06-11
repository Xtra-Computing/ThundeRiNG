#!/bin/bash

source /opt/xilinx/xrt/setup.sh
source /opt/Xilinx/Vitis/2020.2/settings64.sh

make cleanall

nohup  bash -c "make all -j16" &