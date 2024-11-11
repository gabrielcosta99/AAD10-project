//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// MD5 hash CPU code
//
// md5_cpu() -------- compute the MD5 hash of a message
// test_md5_cpu() --- test the correctness of md5_cpu() and measure its execution time
//

#ifndef MD5_CPU
#define MD5_CPU

//
// CPU-only implementation (assumes a little-endian CPU)
//

static void md5_cpu(u32_t *data,u32_t *hash)
{ // one message -> one MD5 hash
  u32_t a,b,c,d,state[4],x[16];
# define C(c)         (c)
# define ROTATE(x,n)  (((x) << (n)) | ((x) >> (32 - (n))))
# define DATA(idx)    data[idx]
# define HASH(idx)    hash[idx]
# define STATE(idx)   state[idx]
# define X(idx)       x[idx]
  CUSTOM_MD5_CODE();
# undef C
# undef ROTATE
# undef DATA
# undef HASH
# undef STATE
# undef X
}


//
// correctness test of md5_cpu()
//

static void test_md5_cpu(void)
{
# define N_MD5SUM_TESTS  64u
# define N_TIMING_TESTS  1000000u
  u32_t n,idx,*htd,*hth;
  char buffer[64],*e;
  FILE *fp;

  htd = &host_md5_test_data[0u];
  hth = &host_md5_test_hash[0u];
  //
  // do all messages
  //
  for(n = 0u;n < N_MESSAGES;n++)
  {
    //
    // compute the MD5 hash
    //
    md5_cpu(htd,hth);
    //
    // compare the MD5 hash with the output of the md5sum command
    //
    if(n < N_MD5SUM_TESTS)
    {
      fp = fopen("/tmp/hash.data","wb");
      if(fp == NULL || fwrite((void *)htd,(size_t)1,(size_t)(13 * 4),fp) != (size_t)(13 * 4) || fclose(fp) != 0)
      {
        remove("/tmp/hash.data");
        fprintf(stderr,"test_md5_cpu: unable to create file \"/tmp/hash.data\"\n");
        exit(1);
      }
      fp = popen("md5sum /tmp/hash.data","r");
      if(fp == NULL || fread(buffer,(size_t)1,(size_t)33,fp) != (size_t)33 || buffer[32u] != ' ' || pclose(fp) != 0)
      {
        remove("/tmp/hash.data");
        fprintf(stderr,"test_md5_cpu: error while running the md5sum command\n");
        exit(1);
      }
      for(idx = 4u;idx > 0u;idx--)
      {
        buffer[8u * idx] = '\0';
        if(strtoul(&buffer[8u * (idx - 1u)],&e,16u) != (unsigned long)SWAP(hth[idx - 1u]) || *e != '\0')
        {
          remove("/tmp/hash.data");
          fprintf(stderr,"test_md5_cpu: MD5 hash error for message %u\n",n);
          exit(1);
        }
      }
    }
    //
    // advance to the next message
    //
    htd = &htd[13u];
    hth = &hth[ 4u];
  }
  //
  // clean up
  //
  remove("/tmp/hash.data");
  //
  // measure the execution time of mp5_cpu()
  //
# if N_TIMING_TESTS > 0u
  time_measurement();
  for(n = 0u;n < N_TIMING_TESTS;n++)
    md5_cpu(&host_md5_test_data[0u],&host_md5_test_hash[0u]);
  time_measurement();
  printf("time per md5 hash ( cpu): %7.3fns %7.3fns\n",cpu_time_delta_ns() / (double)N_TIMING_TESTS,wall_time_delta_ns() / (double)N_TIMING_TESTS);
# endif
# undef N_MD5SUM_TESTS
# undef N_TIMING_TESTS
}

#endif
