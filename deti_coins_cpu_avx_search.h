//
// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_avx_search() --- find DETI coins using md5_cpu_avx()
//

#ifndef DETI_COINS_CPU_AVX_SEARCH
#define DETI_COINS_CPU_AVX_SEARCH

#define N_LANES 4

typedef union{
  u32_t coin_as_ints[13]; // 13 4-byte integers
  char coin_as_chars[52]; // 52 = 13*4 bytes
}coin_t;


static void deti_coins_cpu_avx_search(u32_t n_random_words)
{
  coin_t coin;
  // if(snprintf(coin.coin_as_chars,52,"DETI coin bla, bla, bla\n") != 52){
  //   fprintf(stderr,"not exactly 52 bytes...\n");
  //   exit(0);
  // }
  // for(int idx = 0; idx< 13; idx++){

  // }

  // u32_t n,idx,coin[13u],hash[4u];
  u32_t n,idx,i,lane,v1,v2;
  u64_t n_attempts,n_coins;
  u08_t *bytes;
  int flag=0;
  static u32_t coins_array[13* N_LANES] __attribute__((aligned(16)));
  static u32_t hash_array[ 4u * N_LANES] __attribute__((aligned(16)));

  idx = 6;
  for( lane = 0; lane < N_LANES; lane++) {
    // bytes = (u08_t *)&coins_array[lane*4];
    //
    // mandatory for a DETI coin
    // //
    // coins_array[0u][lane] = 0x49544544; //  "DETI"
    // coins_array[1u][lane] = 0x696f6320; //  " coi"
    // coins_array[2u][lane] = 0x2020206E; //  "n   "
    // coins_array[12u][lane] = 0x0A202020; // "   \n"

    coins_array[0u*N_LANES+lane] = 0x49544544; //  "DETI"
    coins_array[1u*N_LANES+lane] = 0x696f6320; //  " coi"
    coins_array[2u*N_LANES+lane] = 0x2020206E; //  "n   "
    coins_array[12u*N_LANES+lane] = 0x0A202020; // "   \n"

    //
    // arbitrary, but printable utf-8 data terminated with a '\n' is hightly desirable
    //
    //
    // prepare 4 combination (byte range: 0x20..0x7E) for the coins 
    //
    for(i = 3u;i < 12u;i++)
      coins_array[i*N_LANES+lane] = 0x20202020;

    coins_array[idx*N_LANES+lane] = coins_array[idx*N_LANES+lane] + lane; //add something different according to the lane
    // coins_array[2u*N_LANES+lane] = coins_array[2u*N_LANES+lane] + 0x48000000+lane*0x01000000 ;

    // for(idx = 10u;idx < 13u * 4u - 1u;idx++)
    //   coin.coin_as_chars[idx] = ' ';
    // if(lane!= 0){
    //   coin.coin_as_chars[10] +=idx;   // add variety in the first 4 lanes
    // }
    

    // for(i = 0u;i < 13u;i++)                                      // for each message number
    //   coins_array[i*N_LANES+lane] = coin.coin_as_ints[i];
    

    //
    // mandatory termination
    //
    // bytes[13u * 4u -  1u] = '\n';
  }

  //
  // find DETI coins
  //
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=4)
  {
    // if(n_attempts == 10000){     // debug
    //   exit(0);
    // }
    
    //
    // compute MD5 hash
    //
    md5_cpu_avx((v4si *)coins_array,(v4si *)hash_array);
    for(lane = 0; lane<4u; lane++){
      v1 = coins_array[idx*N_LANES+lane];
      v2 = coins_array[(idx+1)*N_LANES+lane];
      u32_t hash[4u]; //= hash_array[4u*i];
      coin_t coin; //= coins_array[13u*i];
      // bytes = (u08_t *)&coins_array[13u*lane];
      for(i = 0u;i < 13u;i++)                                      // for each message number
        // coin[i] = coins_array[13u*lane+i];
        coin.coin_as_ints[i] = coins_array[i*N_LANES+lane];
      for(i = 0u; i<4u; i++)
        hash[i] = hash_array[i*N_LANES+lane];

      // printf("coin: %s\n",coin.coin_as_chars);
      
      // if(snprintf(coin.coin_as_chars,52,"DETI coin bla, bla, bla                           \n") != 52){
      //   fprintf(stderr,"not exactly 52 bytes... %d \n",snprintf(coin.coin_as_chars,52,"DETI coin bla, bla, bla\n"));
      //   printf("coin: %s\n",coin.coin_as_chars);

      //   exit(0);
      // }
      
      // for(int i = 0; i<13u;i++){
      //   printf("%d, ",coin[i]);
      // }
      // printf("\n");
      // printf("hash:   ");
      // for(int i = 0; i<4u;i++){
      //   printf("%d ",hash[i]);
      // }
      // printf("\n");

      // printf("cpuHash: ");
      // u32_t cpuHash[4u];
      // md5_cpu(coin,cpuHash);
      // hash_byte_reverse(cpuHash);
      // for(int i = 0; i<4u;i++){
      //   printf("%d ",cpuHash[i]);
      // }
      // printf("\n");

      //
      // byte-reverse each word (that's how the MD5 message digest is printed...)
      //
      hash_byte_reverse(hash);
      //
      // count the number of trailing zeros of the MD5 hash
      //
      n = deti_coin_power(hash);
      //
      // if the number of trailing zeros is >= 32 we have a DETI coin
      //
      if(n >= 32u)
      {
        save_deti_coin(coin.coin_as_ints);
        n_coins++;
      }
      //
      // try next combination (byte range: 0x20..0x7E)
      // //
      //FINISH THIS

      // for(idx = 10u;idx < 13u * 4u - 1u && coin.coin_as_chars[idx] == (u08_t)126;idx++)
      //   coin.coin_as_chars[idx] = ' ';
      // if(idx < 13u * 4u - 1u)
      //   coin.coin_as_chars[idx] +=4;   // add 4 because we have 4 lanes
    


      // coins_array[2u*N_LANES+lane] = coins_array[2u*N_LANES+lane] | 0x00002020;     // leave the first bytes "n " in their place
      // for(idx = 3u;idx < 12u;idx++)
      //   coins_array[i*N_LANES+lane] = 0x20202020;
      // if(idx < 13u * 4u - 1u){

      //   coins_array[i*N_LANES+lane] = 0x30202020 + 0x010000*lane;
      // }

      
      // for(idx = 10u;idx < 13u * 4u - 1u && bytes[idx] == (u08_t)126;idx++)
      //   bytes[idx] = ' ';
      // if(idx < 13u * 4u - 1u)
      //   bytes[idx] +=4;   // add 4 because we have 4 lanes
      // u32_t prevV1 =v1;
      v1 = next_value_to_try_ascii(v1+N_LANES-1);
      if(v1 == 0x20202020){
        v2 = next_value_to_try_ascii(v2);
        v1 +=lane;
        // printf("prevV1:%X\n",prevV1);
        // printf("v1:%X\n",v1);        
        // printf("v2:%X\n",v2);
        // printf("%s\n",coin.coin_as_chars);
        // flag=5;
      }
      // printf("v1: %X\n",v1);
      // printf("v2: %X\n",v2);
      coins_array[idx*N_LANES + lane] = v1;
      coins_array[(idx+1)*N_LANES + lane] = v2;
      // if(flag>0){
      //   printf("v1:%X\n",v1);
      //   // printf("v2:%X\n",v2);
      //   flag -=1;
      // }
    }
   
    
    
    
  }
  // printf("v1:%X\n",v1);
  // printf("v2:%X\n",v2);
  STORE_DETI_COINS();
  printf("deti_coins_cpu_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
}

#endif
