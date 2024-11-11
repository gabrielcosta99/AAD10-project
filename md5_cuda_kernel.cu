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

extern "C" __global__ __launch_bounds__(128,1) void cuda_md5_kernel(u32_t *interleaved32_data,u32_t *interleaved32_hash)
{
  u32_t n,a,b,c,d,state[4],x[16];

  //
  // get the global thread number
  //
  n = (u32_t)threadIdx.x + (u32_t)blockDim.x * (u32_t)blockIdx.x;
  //
  // adjust data and hash pointers
  //
  interleaved32_data = &interleaved32_data[(n >> 5u) * (32u * 13u) + (n & 31u)];
  interleaved32_hash = &interleaved32_hash[(n >> 5u) * (32u *  4u) + (n & 31u)];
  //
  // compute MD5 hash
  //
# define C(c)         (c)
# define ROTATE(x,n)  (((x) << (n)) | ((x) >> (32 - (n))))
# define DATA(idx)    interleaved32_data[32u * (idx)]
# define HASH(idx)    interleaved32_hash[32u * (idx)]
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
