#!/bin/bash

prj_file=""
if [ -e "_x/link/vivado/prj/prj.xpr" ]
then
prj_file=" _x/link/vivado/prj/prj.xpr"
fi

if [ -e "_x/link/vivado/vpl/prj/prj.xpr" ]
then
prj_file="_x/link/vivado/vpl/prj/prj.xpr"
fi


if [ -e "_x.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/link/vivado/vpl/prj" ]
then
prj_file="_x.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/link/vivado/vpl/prj/prj.xpr"
fi



if [ -z ${prj_file} ]; then
    echo "no prj file"
else
	vivado -mode batch -source ../script/report_usage.tcl -tclargs ${prj_file}
fi
