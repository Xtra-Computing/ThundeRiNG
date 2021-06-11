#!/bin/bash


name=$1

gen_one_seq() {

g++  corr_${name}.c ../pcg_basic/pcg_basic.c -I ../../  -I ../  -I ../pcg_basic/ -o gen_seq_${name}_$1.app -DTARGET_SEQ=$1

./gen_seq_${name}_$1.app > ${name}_seq_$1.csv  &

}

#gen_one_seq 1
#exit
rm -rf *.app


for i in {1..2}
do
    gen_one_seq $i
done

#exit

wait

rm -rf ${name}_cov.log

for i in {1..2}
do
    cp ${name}_seq_$i.csv seq_1.csv
    for j in {1..2}
    do
        cp ${name}_seq_$j.csv seq_2.csv
        echo "[$i,$j] `python3  corrcoef.sh ${name}`" >> ${name}_cov.log

    done
done

#gen_one_seq  1
