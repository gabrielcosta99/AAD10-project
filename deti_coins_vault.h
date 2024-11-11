//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// save_deti_coin() --- save a DETI coin in a temporary buffer; with a NULL argument, update the DETI coins file vault
//

#ifndef DETI_COINS_VAULT
#define DETI_COINS_VAULT

#define DETI_COINS_VAULT_FILE  "deti_coins_vault.txt"

#define STORE_DETI_COINS()  save_deti_coin(NULL)

static void save_deti_coin(u32_t coin[13])
{
# define MAX_SAVED_DETI_COINS 65536u
  static u32_t saved_deti_coins[MAX_SAVED_DETI_COINS * 14u];
  static u32_t n_saved_deti_coins = 0u;
  static u08_t deti_coin_template[52u] =
  {
    [ 0u] = (u08_t)'D',
    [ 1u] = (u08_t)'E',
    [ 2u] = (u08_t)'T',
    [ 3u] = (u08_t)'I',
    [ 4u] = (u08_t)' ',
    [ 5u] = (u08_t)'c',
    [ 6u] = (u08_t)'o',
    [ 7u] = (u08_t)'i',
    [ 8u] = (u08_t)'n',
    [ 9u] = (u08_t)' ',
    [51u] = (u08_t)'\n'
  };
  u32_t idx,n,header,hash[4];
  FILE *fp;

  //
  // handle a NULL argument (meaning: save the DETI coins) or a full buffer
  //
  if(coin == NULL || n_saved_deti_coins == MAX_SAVED_DETI_COINS)
  {
    if(n_saved_deti_coins > 0u)
    {
      fp = fopen(DETI_COINS_VAULT_FILE,"a");
      if(fp == NULL                                                                                                        ||
         fwrite((void *)&saved_deti_coins[0],(size_t)(14 * 4),(size_t)n_saved_deti_coins,fp) != (size_t)n_saved_deti_coins ||
         fflush(fp) != 0                                                                                                   ||
         fclose(fp) != 0)
      {
        fprintf(stderr,"save_deti_coin: unable to update file \"" DETI_COINS_VAULT_FILE "\"\n");
        exit(1);
      }
    }
    n_saved_deti_coins = 0u;
  }
  if(coin == NULL)
    return;
  //
  // make sure that the coin has the appropriate format
  //
  for(idx = 0u;idx < 52u;idx++)
    if(deti_coin_template[idx] != (u08_t)0 && deti_coin_template[idx] != ((u08_t *)coin)[idx])
    {
      fprintf(stderr,"save_deti_coin: bad DETI coin format\n");
      for(idx = 0u;idx < 52u;idx++)
        if(deti_coin_template[idx] != (u08_t)0)
          fprintf(stderr,"%2d 0x%02X 0x%02X %s\n",idx,(u32_t)(((u08_t *)coin)[idx]),(u32_t)deti_coin_template[idx],(deti_coin_template[idx] != ((u08_t *)coin)[idx]) ? "!=" : "==");
        else
          fprintf(stderr,"%2d 0x%02X\n",idx,(u32_t)(((u08_t *)coin)[idx]));
      exit(1);
    }
  //
  // compute MD5 hash
  //
  md5_cpu(coin,hash);
  //
  // byte-reverse each word (that's how the MD5 message digest is printed...)
  //
  hash_byte_reverse(hash);
  //
  // count the number of trailing zeros of the MD5 hash
  //
  n = deti_coin_power(hash);
  if(n < 32u)
  {
    fprintf(stderr,"save_deti_coin: number of zero bits (%u) is too small\n",n);
    exit(1);
  }
  //
  // save the DETI coin in the buffer; the value of coin is the number of trailing zeros minus 32
  // format of each line: "Vuv:" "coin_data" where u and v are ascii digits that encode, in base 10, the reported power of the DETI coin
  //
  n -= 32u;
  header = ((u32_t)'V' << 0) | (((u32_t)'0' + n / 10u) << 8) | (((u32_t)'0' + n % 10u) << 16) | ((u32_t)':'  << 24);
  n = 14u * n_saved_deti_coins++;
  saved_deti_coins[n] = header;
  for(idx = 0u;idx < 13u;idx++)
    saved_deti_coins[n + 1u + idx] = coin[idx];
# undef MAX_SAVED_DETI_COINS
}

#endif
