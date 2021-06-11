#!/bin/bash


gen_one_seq() {

gcc  corr_xor32.c -o  gen_seq_xor32_$1.app -DTARGET_SEQ=$1

./gen_seq_xor32_$1.app > xor32_seq_$1.csv  &

}

rm -rf *.app


for i in {1..4}
do
    gen_one_seq $i
done


wait

rm -rf cov.log

for i in {1..4}
do
    cp xor32_seq_$i.csv seq_1.csv
    for j in {1..4}
    do
        cp xor32_seq_$j.csv seq_2.csv
        echo "[$i,$j] `python3  corrcoef.py`" >> cov.log
        exit

    done
done

#gen_one_seq  1
