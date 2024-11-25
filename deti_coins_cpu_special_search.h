//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// deti_coins_cpu_search() --- find DETI coins using md5_cpu()
//

#ifndef DETI_COINS_CPU_SPECIAL_SEARCH
#define DETI_COINS_CPU_SPECIAL_SEARCH

static void deti_coins_cpu_special_search(void)
{
  u32_t n,idx,hash[4u];
  u64_t n_attempts,n_coins;
  coin_t coin;

  
  //
  // mandatory for a DETI coin
  //
  coin.coin_as_chars[0u] = 'D';
  coin.coin_as_chars[1u] = 'E';
  coin.coin_as_chars[2u] = 'T';
  coin.coin_as_chars[3u] = 'I';
  coin.coin_as_chars[4u] = ' ';
  coin.coin_as_chars[5u] = 'c';
  coin.coin_as_chars[6u] = 'o';
  coin.coin_as_chars[7u] = 'i';
  coin.coin_as_chars[8u] = 'n';
  coin.coin_as_chars[9u] = ' ';
  coin.coin_as_chars[10u] = 'a';
  coin.coin_as_chars[11u] = ' ';
  coin.coin_as_chars[12u] = 't';
  coin.coin_as_chars[13u] = 'u';
  coin.coin_as_chars[14u] = 'a';
  coin.coin_as_chars[15u] = ' ';
  coin.coin_as_chars[16u] = 'm';
  coin.coin_as_chars[17u] = 'a';
  coin.coin_as_chars[18u] = 'e';
  coin.coin_as_chars[19u] = ':';
  coin.coin_as_chars[20u] = ' ';
  // coin.coin_as_chars[21u] = ' ';
  // coin.coin_as_chars[22u] = ' ';
  // coin.coin_as_chars[23u] = 'n';
  // coin.coin_as_chars[24u] = 'o';
  // coin.coin_as_chars[25u] = ':';
  // coin.coin_as_chars[26u] = ' ';
 
  

  //
  // arbitrary, but printable utf-8 data terminated with a '\n' is hightly desirable
  //
  for(idx = 21u;idx < 13u * 4u - 1u;idx++)
    coin.coin_as_chars[idx] = '0';
  //
  // mandatory termination
  //
  coin.coin_as_chars[13u * 4u -  1u] = '\n';
  //
  // find DETI coins
  //
  for(n_attempts = n_coins = 0ul;stop_request == 0;n_attempts++)
  {
    //
    // compute MD5 hash
    //
    md5_cpu(coin.coin_as_ints,hash);
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
    //
    for(idx = 13u * 4u - 2u; idx > 21u && coin.coin_as_chars[idx] == '9';idx--)
      coin.coin_as_chars[idx] = '0';
    // printf("idx: %d\n",idx);
    if(idx > 21u)
      coin.coin_as_chars[idx]++;
    // printf("coin: %s, idx:%d\n",coin.coin_as_chars,idx);
  }
  STORE_DETI_COINS();
  printf("deti_coins_cpu_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",n_coins,(n_coins == 1ul) ? "" : "s",n_attempts,(n_attempts == 1ul) ? "" : "s",(double)n_attempts / (double)(1ul << 32));
}

#endif
