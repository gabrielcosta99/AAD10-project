//
// Arquiteturas de Alto Desempenho 2024/2025
//
// MD5 hash CPU code using AVX2 instructions (Intel/AMD)
//
// md5_cpu_avx2() -------- compute the MD5 hash of a message
// test_md5_cpu_avx2() --- test the correctness of md5_cpu() and measure its execution time
//

#if defined(__GNUC__) && defined(__AVX2__)
#ifndef MD5_CPU_AVX2
#define MD5_CPU_AVX2

//
// CPU-only implementation using AVX2 instructions (assumes a little-endian CPU)
//

typedef int v8si __attribute__ ((vector_size (32)));

static void md5_cpu_avx2(v8si *interleaved8_data,v8si *interleaved8_hash)
{ // eight interleaved messages -> eight interleaved MD5 hashes
  v8si a,b,c,d,interleaved8_state[4],interleaved8_x[16];
# define C(c)         (v8si){ (int)(c),(int)(c),(int)(c),(int)(c),(int)(c),(int)(c),(int)(c),(int)(c) }
# define ROTATE(x,n)  (__builtin_ia32_pslldi256(x,n) | __builtin_ia32_psrldi256(x,32 - (n)))
# define DATA(idx)    interleaved8_data[idx]
# define HASH(idx)    interleaved8_hash[idx]
# define STATE(idx)   interleaved8_state[idx]
# define X(idx)       interleaved8_x[idx]
  CUSTOM_MD5_CODE();
# undef C
# undef ROTATE
# undef DATA
# undef HASH
# undef STATE
# undef X
}


//
// correctness test of md5_cpu_avx2() --- test_md5_cpu() must be called first!
//

static void test_md5_cpu_avx2(void)
{
# define N_TIMING_TESTS  1000000u
  static u32_t interleaved_test_data[13u * 8u] __attribute__((aligned(32)));
  static u32_t interleaved_test_hash[ 4u * 8u] __attribute__((aligned(32)));
  u32_t n,lane,idx,*htd,*hth;
  if(N_MESSAGES % 8u != 0u)
  {
    fprintf(stderr,"test_md5_cpu_avx2: N_MESSAGES is not a multiple of 8\n");
    exit(1);
  }
  htd = &host_md5_test_data[0u];
  hth = &host_md5_test_hash[0u];
  for(n = 0u;n < N_MESSAGES;n += 8u)
  {
    //
    // interleave data
    //
    for(lane = 0u;lane < 8u;lane++)                                      // for each message number
      for(idx = 0u;idx < 13u;idx++)                                      //  for each message word
        interleaved_test_data[8u * idx + lane] = htd[13u * lane + idx];  //   interleave
    //
    // compute MD5 hashes
    //
    md5_cpu_avx2((v8si *)interleaved_test_data,(v8si *)interleaved_test_hash);
    //
    // compare with the test_md5_cpu() data
    //
    for(lane = 0u;lane < 8u;lane++)  // for each message number
      for(idx = 0u;idx < 4u;idx++)   //  for each hash word
        if(interleaved_test_hash[8u * idx + lane] != hth[4u * lane + idx])
        {
          fprintf(stderr,"test_md5_cpu_avx2: MD5 hash error for message %u\n",8u * n + lane);
          exit(1);
        }
    //
    // advance to the next 8 messages
    //
    htd = &htd[13u * 8u];
    hth = &hth[ 4u * 8u];
  }
  //
  // measure the execution time of mp5_cpu_avx()
  //
# if N_TIMING_TESTS > 0u
  time_measurement();
  for(n = 0u;n < N_TIMING_TESTS;n++)
    md5_cpu_avx2((v8si *)interleaved_test_data,(v8si *)interleaved_test_hash);
  time_measurement();
  printf("time per md5 hash ( avx2): %7.3fns %7.3fns\n",cpu_time_delta_ns() / (double)(8u * N_TIMING_TESTS),wall_time_delta_ns() / (double)(8u * N_TIMING_TESTS));
# endif
# undef N_TIMING_TESTS
}

#endif
#endif
