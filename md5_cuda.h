//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// MD5 hash CUDA code
//
// md5_cuda() -------- compute the MD5 hash of n messages
// test_md5_cuda() --- test the correctness of md5_cpu() and measure its execution time
//

#if USE_CUDA > 0
#define MD5_CUDA

static void md5_cuda(u32_t *data,u32_t *hash,u32_t n_messages)
{
  u32_t n,lane,idx,*ptr;
  void *cu_params[2];

  if(n_messages % 128u != 0u)
  {
    fprintf(stderr,"initialize_cuda: n_messages is not a multiple of 128\n");
    exit(1);
  }
  //
  // data -> host_data with 32-way interleaving
  //
  ptr = host_data;
  for(n = 0u;n < n_messages;n += 32u)
  {
    for(lane = 0u;lane < 32u;lane++)                     // for each message number
      for(idx = 0u;idx < 13u;idx++)                      //  for each message word
        ptr[32u * idx + lane] = data[13u * lane + idx];  //   interleave
    data = &data[13u * 32u];
    ptr = &ptr[13u * 32u];
  }
  //
  // launch kernel
  // * transfer data[] from the host to the device
  // * call CUDA kernel
  // * transfer hash[] from the device to the host
  //
  CU_CALL( cuMemcpyHtoD , (device_data,(void *)host_data,(size_t)n_messages * (size_t)13 * sizeof(u32_t)) );
  cu_params[0] = &device_data;
  cu_params[1] = &device_hash;
  CU_CALL( cuLaunchKernel , (cu_kernel,n_messages / 128u,1u,1u,128u,1u,1u,0u,(CUstream)0,&cu_params[0],NULL) );
  CU_CALL( cuMemcpyDtoH , ((void *)host_hash,device_hash,(size_t)n_messages * (size_t)4 * sizeof(u32_t)) );
  CU_CALL( cuStreamSynchronize , (0) );
  //
  // host_hash -> hash with 32-way interleaving
  //
  ptr = host_hash;
  for(n = 0u;n < n_messages;n += 32u)
  {
    for(lane = 0u;lane < 32u;lane++)                    // for each message number
      for(idx = 0u;idx < 4u;idx++)                      //  for each message word
        hash[4u * lane + idx] = ptr[32u * idx + lane];  //   deinterleave
    ptr = &ptr[4u * 32u];
    hash = &hash[4u * 32u];
  }
}


//
// correctness test of md5_cuda()
//

static void test_md5_cuda(void)
{
  u32_t n,idx,hash[4];

  initialize_cuda(0,"md5_cuda_kernel.cubin","cuda_md5_kernel",13u * N_MESSAGES,4u * N_MESSAGES);
  time_measurement();
  md5_cuda(host_md5_test_data,host_md5_test_hash,N_MESSAGES);
  time_measurement();
  terminate_cuda();
  for(n = 0u;n < N_MESSAGES;n++)
  {
    md5_cpu(&host_md5_test_data[13u * n],&hash[0u]);
    for(idx = 0u;idx < 4u;idx++)
      if(host_md5_test_hash[4u * n + idx] != hash[idx])
      {
        fprintf(stderr,"test_md5_cuda: MD5 hash error for message %u\n",n);
        exit(1);
      }
  }
  printf("time per md5 hash (CUDA): %7.3fns %7.3fns\n",cpu_time_delta_ns() / (double)N_MESSAGES,wall_time_delta_ns() / (double)N_MESSAGES);
}

#endif
