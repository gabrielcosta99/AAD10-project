
#include <stdio.h>
#include <time.h>
#include "md5.h"
typedef unsigned int u32_t;


#define next_value_to_try(v1) \
    do { \
        v1++; \
        if ((v1 & 0xFF) == 0x7F) { \
        v1 += 0xA1; \
        if (((v1 >> 8) & 0xFF) == 0x7F) { \
            v1 += 0xA1 << 8; \
            if (((v1 >> 16) & 0xFF) == 0x7F) { \
            v1 += 0xA1 << 16; \
            if (((v1 >> 24) & 0xFF) == 0x7F) { \
                v1 += 0xA1 << 24; \
            } \
            } \
        } \
        } \
    } while(0)


int main(void) 
{
    u32_t coin[13u],hash[4u];
    u32_t n_attempts,n_coins,v1,v2;
    u32_t a,b,c,d,state[4],x[16];
    clock_t t;

    coin[0]=0x49544544;
    coin[1]=0x696f6320;
    coin[2]=0x6e20206e;
    coin[3]=0x65626d75;
    coin[4]=0x666f2072;
    coin[5]=0x65687420;
    coin[6]=0x74746120;
    coin[7]=0x74706d65;
    coin[8]=0x305b2020;
    coin[9]=0x30303030;
    v1=coin[10]=0x33333530;
    v2=coin[11]=0x35383238;
    coin[12]=0x0a5d3137;


    t = clock();
    for(n_attempts = n_coins = 0; n_attempts < 1000000000u; n_attempts++)
    {
        
# define C(c)         (c)
# define ROTATE(x,n)  (((x) << (n)) | ((x) >> (32 - (n))))
# define DATA(idx)    coin[idx]
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
    if(hash[3] == 0) 
    {
        for(int i = 0; i< 13; i++) 
        {
            printf("%08X%c",coin[i], (i < 12) ? ' ' : '\n');
        }
        n_coins++;
    }
    next_value_to_try(v1); coin[10] = v1;
    if(v1 == 0x20202020) { next_value_to_try(v2); coin[11] = v2;}
    }
    t = clock() - t;
    printf("deti_coins with web assembly: %u DETI coin%s found in %u attempt%s (expected %.2f coins) in %.3fs\n",n_coins,(n_coins == 1u) ? "" : "s",n_attempts,(n_attempts == 1u) ? "" : "s",(double)n_attempts / (double)(1ull << 32), (double)t/CLOCKS_PER_SEC);
}

