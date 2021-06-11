template <typename RealType>
bool run_skip_ahead_example(sycl::queue& queue) {
    int NCPUs = sysconf(_SC_NPROCESSORS_CONF);
    printf("Using thread affinity on %i NCPUs\n", NCPUs);

    std::cout << "\n\tRun skip_ahead example" << std::endl;

    oneapi::mkl::rng::philox4x32x10 engine(queue);

    std::vector<oneapi::mkl::rng::philox4x32x10> engine_vec;

    for (int i = 0; i < S; i++) {
        // copy reference engine to engine_vec[i]
        engine_vec.push_back(oneapi::mkl::rng::philox4x32x10{engine});
        // skip ahead engine
        oneapi::mkl::rng::skip_ahead(engine_vec[i], i * NS);
    }


    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r_ref(N);
    std::vector<RealType, mkl_allocator<RealType, 64>> r(NS * S);

    {
        sycl::buffer<RealType, 1> r_ref_buffer(r_ref.data(), r_ref.size());

        try {
            {
                // fill r_ref with N random numbers
                double startStamp, endStamp;
                startStamp = getCurrentTimestamp();
                oneapi::mkl::rng::generate(oneapi::mkl::rng::uniform<RealType> {}, engine, N, r_ref_buffer);
                queue.wait_and_throw();
                endStamp = getCurrentTimestamp();
                std::cout << "\t\tSingle Output:" << endStamp - startStamp << std::endl;
            }

            // fill r with random numbers by portions of NS

            {

                double startStamp, endStamp;
                std::vector<sycl::buffer<RealType, 1>>  vector_r_buffer;
                for (int i = 0; i < S; i++) {
                    sycl::buffer<RealType, 1> r_buffer(r.data() + i * NS, NS);
                    vector_r_buffer.push_back(r_buffer);
                }

                startStamp = getCurrentTimestamp();
                #pragma omp parallel
                {
                    cpu_set_t new_mask;
                    cpu_set_t was_mask;
                    int tid = omp_get_thread_num();

                    CPU_ZERO(&new_mask);

                    // 2 packages x 2 cores/pkg x 1 threads/core (4 total cores)
                    CPU_SET( tid, &new_mask);

                    if (sched_getaffinity(0, sizeof(was_mask), &was_mask) == -1) {
                        printf("Error1: sched_getaffinity(%d, sizeof(was_mask), &was_mask)\n", tid);
                    }
                    if (sched_setaffinity(0, sizeof(new_mask), &new_mask) == -1) {
                        printf("Error2: sched_setaffinity(%d, sizeof(new_mask), &new_mask)\n", tid);
                    }
                    if (sched_getaffinity(0, sizeof(new_mask), &new_mask) == -1) {
                        printf("Error1: sched_getaffinity(%d, sizeof(was_mask), &was_mask)\n", tid);
                    }
                    printf("tid=%d new_mask=%08X was_mask=%08X\n", tid,
                           *(unsigned int*)(&new_mask), *(unsigned int*)(&was_mask));
                    int i = omp_get_thread_num();
                    oneapi::mkl::rng::generate(oneapi::mkl::rng::uniform<RealType> {}, engine_vec[i], NS, vector_r_buffer[i]);
                }
                queue.wait_and_throw();
                endStamp = getCurrentTimestamp();
                std::cout << "\t\tMultiple Output:" << endStamp - startStamp << std::endl;
            }
        }
        catch (sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }
    }
    std::cout << "Success" << std::endl;
    return true;
}
