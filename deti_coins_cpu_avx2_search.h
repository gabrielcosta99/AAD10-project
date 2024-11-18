//
// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_avx_search() --- find DETI coins using md5_cpu_avx()
//

#ifndef DETI_COINS_CPU_AVX2_SEARCH
#define DETI_COINS_CPU_AVX2_SEARCH

static void deti_coins_cpu_avx2_search(u32_t n_random_words)
{
  // u32_t n,idx,coin[13u],hash[4u];
  u32_t n,idx;
  u64_t n_attempts,n_coins;
  u08_t *bytes;

  u32_t coins_array[13u * 8u] __attribute__((aligned(32)));
  u32_t hash_array[ 4u * 8u] __attribute__((aligned(32)));

  for(int i = 0; i < 8u; i++) {
    bytes = (u08_t *)&coins_array[13u*i];
    //
    // mandatory for a DETI coin
    //
    bytes[0u] = 'D';
    bytes[1u] = 'E';
    bytes[2u] = 'T';
    bytes[3u] = 'I';
    bytes[4u] = ' ';
    bytes[5u] = 'c';
    bytes[6u] = 'o';
    bytes[7u] = 'i';
    bytes[8u] = 'n';
    bytes[9u] = ' ';
    //
    // arbitrary, but printable utf-8 data terminated with a '\n' is hightly desirable
    //
    for(idx = 10u;idx < 13u * 4u - 1u;idx++)
      bytes[idx] = ' ';
    //
    // mandatory termination
    //
    bytes[13u * 4u -  1u] = '\n';
  }

  //
  // find DETI coins
  //
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts+=8)
  {
    //
    // compute MD5 hash
    //
    md5_cpu_avx2((v8si *)coins_array,(v8si *)hash_array);
    
    for(int lane = 0; lane<8u; lane++){
      u32_t hash[4u]; //= hash_array[4u*i];
      u32_t coin[13u]; //= coins_array[13u*i];
      bytes = (u08_t *)&coins_array[13u*lane];
      for(int i = 0u;i < 13u;i++)                                      // for each message number
        coin[i] = coins_array[13u*lane+i];
      for(int i = 0u; i<4u; i++)
        hash[i] = hash_array[4u*lane+i];
      
      //
      // byte-reverse each word (that's how the MD5 message digest is printed...)
      //
      hash_byte_reverse(hash);

      // u32_t target_hash[4u]; //= hash_array[4u*i];
      // md5_cpu(coin,target_hash);
      // hash_byte_reverse(target_hash);
      // printf("hash:%d,target_hash:%d\n",*hash,*target_hash);


      // count the number of trailing zeros of the MD5 hash
      //
      n = deti_coin_power(hash);
      //
      // if the number of trailing zeros is >= 32 we have a DETI coin
      //
      // printf("lane: %d, n: %d\n",lane,n);
      if(n >= 32u)
      {
        save_deti_coin(coin);
        n_coins++;
      }
      //
      // try next combination (byte range: 0x20..0x7E)
      //
      for(idx = 10u;idx < 13u * 4u - 1u && bytes[idx] == (u08_t)126;idx++)
        bytes[idx] = ' ';
      if(idx < 13u * 4u - 1u)
        bytes[idx]++;
    }
  }
  STORE_DETI_COINS();
  printf("deti_coins_cpu_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
}

#endif
