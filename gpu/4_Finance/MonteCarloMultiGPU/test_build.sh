



test_one_config() {
make clean
make   TEST_CFLAG="-D__TEST_S__=$1 -D__TEST_MAX_CPU__=$2 -D__TEST_STATE_SHARE__=$3  -D__TEST_THUNDERING_TEST__=$4"

for i in {1..10}
do
   ./MonteCarloMultiGPU 1024 > ../CUDA_$1_$2_$3_$4_at$i.log
done

}


test_one_config 1 2560 0 0
test_one_config 2 2560 0 0
test_one_config 4 2560 0 0
test_one_config 8 2560 0 0
test_one_config 16 2560 0 0
test_one_config 32 2560 0 0
test_one_config 64 2560 0 0
test_one_config 128 2560 0 0
test_one_config 256 2560 0 0
test_one_config 512 2560 0 0
test_one_config 1024 2560 0 0


test_one_config 1 2560 0 1
test_one_config 2 2560 0 1
test_one_config 4 2560 0 1
test_one_config 8 2560 0 1
test_one_config 16 2560 0 1
test_one_config 32 2560 0 1
test_one_config 64 2560 0 1
test_one_config 128 2560 0 1
test_one_config 256 2560 0 1
test_one_config 512 2560 0 1
test_one_config 1024 2560 0 1



test_one_config 1 2560 1 1
test_one_config 2 2560 1 1
test_one_config 4 2560 1 1
test_one_config 8 2560 1 1
test_one_config 16 2560 1 1
test_one_config 32 2560 1 1
test_one_config 64 2560 1 1
test_one_config 128 2560 1 1
test_one_config 256 2560 1 1
test_one_config 512 2560 1 1
test_one_config 1024 2560 1 1
