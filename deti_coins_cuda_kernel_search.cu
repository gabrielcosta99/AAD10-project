//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// MD5 hash CUDA kernel code
//
// md5_cuda_kernel() --- each thread computes the MD5 hash of one message
//
// do not use this directy to search for DETI coins!
//

//
// needed stuff
//

typedef unsigned int u32_t;

#include "md5.h"

//
// the nvcc compiler stores x[] and state[] in registers (constant indices!)
//
// global thread number: n = threadIdx.x + blockDim.x * blockIdx.x
// global warp number: n >> 5
// warp thread number: n & 31
//

// typedef union{
//   u32_t coin_as_ints[13]; // 13 4-byte integers
//   char coin_as_chars[52]; // 52 = 13*4 bytes
// }coin_t;


extern "C" __global__ __launch_bounds__(128,1) void deti_coins_cuda_kernel_search(u32_t *deti_coins_storage_area, u32_t v1, u32_t v2)
{
    u32_t n,a,b,c,d,coin[13],hash[4],state[4],x[16], idx, n1,n2,n3;

    //
    // get the global thread number
    //
    n = (u32_t)threadIdx.x + (u32_t)blockDim.x * (u32_t)blockIdx.x;
    // printf("thread_number: %d\n",n);
    if(n <= 95*95*95){
        // if(n >9000)
        //     printf("n: %d\n",n);
        n3 = n/(95*95);
        n2 = n%(95*95) / 95 ;
        n1 = n % (95);
        // v1+= (n3 << 16) +(n2 << 8) + n1 ;
        // printf("n:%d, n2:%d,n1:%d,v2:%X, v1:%X\n",n,n2,n1,v2,v1);

        coin[0u] = 0x49544544; //  "DETI"
        coin[1u] = 0x696f6320; //  " coi"
        coin[2u] = 0x2020206E; //  "n   "
        coin[12u] = 0x0A202020; // "   \n"
        for(int i = 3u;i < 12u;i++)
            coin[i] = 0x20202020;
        coin[11u] += (n3 << 16) +(n2 << 8) + n1;
        coin[6] = v1;
        coin[7] = v2;
        for(int i = 0; i<95; i++){
//
// compute MD5 hash
//
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


            // idx = atomicAdd(deti_coins_storage_area,13);
            // if(idx<=1000-13)
            //     ...deti_coins_storage_area[idx] = coin[0]
            if(hash[3u] == 0u)
            {
                idx = atomicAdd(deti_coins_storage_area,13);
                if(idx<=15000*13-13){
                    for(int j= 0;j<13;j++){
                        deti_coins_storage_area[idx+j] = coin[j];
                    }

                }
                // printf("idx: %3d,v2:%X,v1:%X\n",idx,v2,v1); // this value should be 1,14,27,etc...
            }
            coin[6u] += 1u;
            // coin[6u] += 1u << 16; // next combination
        }
    }
        
}

