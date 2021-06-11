source /opt/intel/oneapi/setvars.sh intel64




test_one_config() {
rm -r *
cmake ..  -DCMAKE_CXX_FLAGS=" -D__TEST_S__=$1 -D__TEST_MAX_CPU__=$2 -D__TEST_STATE_SHARE__=$3  -D__TEST_THUNDERING_TEST__=$4"
make -j

for i in {1..10}
do
   ./rng-service_functions > ../MKL_$1_$2_$3_$4_at$i.log
done

}


test_one_config 1 96 0 0
test_one_config 2 96 0 0
test_one_config 4 96 0 0
test_one_config 8 96 0 0
test_one_config 16 96 0 0
test_one_config 32 96 0 0
test_one_config 64 96 0 0
test_one_config 128 96 0 0
test_one_config 256 96 0 0
test_one_config 512 96 0 0
test_one_config 1024 96 0 0
test_one_config 2048 96 0 0


test_one_config 1 96 0 1
test_one_config 2 96 0 1
test_one_config 4 96 0 1
test_one_config 8 96 0 1
test_one_config 16 96 0 1
test_one_config 32 96 0 1
test_one_config 64 96 0 1
test_one_config 128 96 0 1
test_one_config 256 96 0 1
test_one_config 512 96 0 1
test_one_config 1024 96 0 1
test_one_config 2048 96 0 1


test_one_config 1 96 1 1
test_one_config 2 96 1 1
test_one_config 4 96 1 1
test_one_config 8 96 1 1
test_one_config 16 96 1 1
test_one_config 32 96 1 1
test_one_config 64 96 1 1
test_one_config 128 96 1 1
test_one_config 256 96 1 1
test_one_config 512 96 1 1
test_one_config 1024 96 1 1
test_one_config 2048 96 1 1
