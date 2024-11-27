//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_search() --- find DETI coins using md5_cpu()
//

#ifndef DETI_COINS_CUDA_SEARCH
#define DETI_COINS_CUDA_SEARCH

static void deti_coins_cuda_search(void)
{
  initialize_cuda(0,"deti_coins_cuda_kernel_search.cubin","deti_coins_cuda_kernel_search",0,10000);

  u32_t i,j,v1,v2;
  u64_t n_attempts,n_coins;

  u32_t host_hash[10000];
  host_hash[0] = 1;
  u32_t n_messages = 95*95*95; // 95**2

  void *cu_params[3];


  // //
  // // mandatory for a DETI coin
  // // 
  // coins_array[0u] = 0x49544544; //  "DETI"
  // coins_array[1u] = 0x696f6320; //  " coi"
  // coins_array[2u] = 0x2020206E; //  "n   "
  // coins_array[12u] = 0x0A202020; // "   \n"

  // //
  // // prepare 8 combination (byte range: 0x20..0x7E) for the coins 
  // //
  // for(i = 3u;i < 12u;i++)
  //   coins_array[i] = 0x20202020;

  //
  // find DETI coins
  //
  CU_CALL( cuMemcpyHtoD , (device_hash,(void *)host_hash, 10000 * sizeof(u32_t)) );
  v1 = 0x20202020;
  v2 = 0x20202020;
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=n_messages)
  {
    //
    // compute MD5 hash
    //
    cu_params[0] = &device_hash;
    cu_params[1] = &v1;
    cu_params[2] = &v2;
    CU_CALL( cuLaunchKernel , (cu_kernel,(n_messages+127) / 128u,1u,1u,128u,1u,1u,0u,(CUstream)0,&cu_params[0],NULL) );
    // CU_CALL( cuMemcpyDtoH , ((void *)host_hash,device_hash,(size_t)n_messages * (size_t)4 * sizeof(u32_t)) );
    CU_CALL( cuStreamSynchronize , (0) );

    v1 += 0x005E5E5E;
    v1 = next_value_to_try_ascii(v1);
    if(v1 == 0x20202020){
      v2 = next_value_to_try_ascii(v2);
    }
    // printf("local_v1: %X\n",v1);
    
  }
  CU_CALL( cuMemcpyDtoH , ((void *)host_hash,device_hash,10000 * sizeof(u32_t)) );
  terminate_cuda();
  n_coins = (host_hash[0]-1)/13;
  printf("n_coins: %ld\n",n_coins);
  coin_t coin;
  for(i = 0; i<n_coins;i++){
    for(j = 0; j<13;j++){
      coin.coin_as_ints[j] = host_hash[i*13+1+j];
    }
    printf("%s\n",coin.coin_as_chars);
    save_deti_coin(coin.coin_as_ints);
  }
  STORE_DETI_COINS();
  printf("deti_coins_cpu_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
}

#endif
