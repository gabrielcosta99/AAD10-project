//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// CUDA driver API stuff
//

#if USE_CUDA > 0
#ifndef CUDA_DRIVER_API_UTILITIES
#define CUDA_DRIVER_API_UTILITIES

#include <cuda.h>


//
// CU_CALL --- macro that should be used to call a CUDA driver API function and to test its return value
//
// it should be used to test the return value of calls such as
//   cuInit(device_number);
//   cuDeviceGet(&cu_device,device_number);
//
// in these cases, f_name is, respectively, cuInit and cuDeviceGet, and args is, respectively,
//   (device_number) and (&cu_device,device_number)
//

#define CU_CALL(f_name,args)                                                                                  \
  do                                                                                                          \
  {                                                                                                           \
    CUresult e = f_name args;                                                                                 \
    if(e != CUDA_SUCCESS)                                                                                     \
    { /* the call failed, terminate the program */                                                            \
      fprintf(stderr,"" # f_name "() returned %s (file %s, line %d)\n",cu_error_string(e),__FILE__,__LINE__); \
      exit(1);                                                                                                \
    }                                                                                                         \
  }                                                                                                           \
  while(0)


//
// "user-friendly" description of the CUDA error codes (replacement of the error code number by its name)
//

static const char *cu_error_string(CUresult e)
{
  static char error_string[64];
# define CASE(error_code)  case error_code: return "" # error_code;
  switch((int)e)
  { // list of error codes extracted from cuda.h (TODO: /usr/local/cuda-10.2/targets/x86_64-linux/include/CL)
    default: sprintf(error_string,"unknown error code (%d)",(int)e); return(error_string);
    CASE(CUDA_SUCCESS                             );
    CASE(CUDA_ERROR_INVALID_VALUE                 );
    CASE(CUDA_ERROR_OUT_OF_MEMORY                 );
    CASE(CUDA_ERROR_NOT_INITIALIZED               );
    CASE(CUDA_ERROR_DEINITIALIZED                 );
    CASE(CUDA_ERROR_PROFILER_DISABLED             );
    CASE(CUDA_ERROR_PROFILER_NOT_INITIALIZED      );
    CASE(CUDA_ERROR_PROFILER_ALREADY_STARTED      );
    CASE(CUDA_ERROR_PROFILER_ALREADY_STOPPED      );
    CASE(CUDA_ERROR_NO_DEVICE                     );
    CASE(CUDA_ERROR_INVALID_DEVICE                );
    CASE(CUDA_ERROR_INVALID_IMAGE                 );
    CASE(CUDA_ERROR_INVALID_CONTEXT               );
    CASE(CUDA_ERROR_CONTEXT_ALREADY_CURRENT       );
    CASE(CUDA_ERROR_MAP_FAILED                    );
    CASE(CUDA_ERROR_UNMAP_FAILED                  );
    CASE(CUDA_ERROR_ARRAY_IS_MAPPED               );
    CASE(CUDA_ERROR_ALREADY_MAPPED                );
    CASE(CUDA_ERROR_NO_BINARY_FOR_GPU             );
    CASE(CUDA_ERROR_ALREADY_ACQUIRED              );
    CASE(CUDA_ERROR_NOT_MAPPED                    );
    CASE(CUDA_ERROR_NOT_MAPPED_AS_ARRAY           );
    CASE(CUDA_ERROR_NOT_MAPPED_AS_POINTER         );
    CASE(CUDA_ERROR_ECC_UNCORRECTABLE             );
    CASE(CUDA_ERROR_UNSUPPORTED_LIMIT             );
    CASE(CUDA_ERROR_CONTEXT_ALREADY_IN_USE        );
    CASE(CUDA_ERROR_PEER_ACCESS_UNSUPPORTED       );
    CASE(CUDA_ERROR_INVALID_PTX                   );
    CASE(CUDA_ERROR_INVALID_GRAPHICS_CONTEXT      );
    CASE(CUDA_ERROR_NVLINK_UNCORRECTABLE          );
    CASE(CUDA_ERROR_INVALID_SOURCE                );
    CASE(CUDA_ERROR_FILE_NOT_FOUND                );
    CASE(CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND);
    CASE(CUDA_ERROR_SHARED_OBJECT_INIT_FAILED     );
    CASE(CUDA_ERROR_OPERATING_SYSTEM              );
    CASE(CUDA_ERROR_INVALID_HANDLE                );
    CASE(CUDA_ERROR_NOT_FOUND                     );
    CASE(CUDA_ERROR_NOT_READY                     );
    CASE(CUDA_ERROR_ILLEGAL_ADDRESS               );
    CASE(CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES       );
    CASE(CUDA_ERROR_LAUNCH_TIMEOUT                );
    CASE(CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING );
    CASE(CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED   );
    CASE(CUDA_ERROR_PEER_ACCESS_NOT_ENABLED       );
    CASE(CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE        );
    CASE(CUDA_ERROR_CONTEXT_IS_DESTROYED          );
    CASE(CUDA_ERROR_ASSERT                        );
    CASE(CUDA_ERROR_TOO_MANY_PEERS                );
    CASE(CUDA_ERROR_HOST_MEMORY_ALREADY_REGISTERED);
    CASE(CUDA_ERROR_HOST_MEMORY_NOT_REGISTERED    );
    CASE(CUDA_ERROR_HARDWARE_STACK_ERROR          );
    CASE(CUDA_ERROR_ILLEGAL_INSTRUCTION           );
    CASE(CUDA_ERROR_MISALIGNED_ADDRESS            );
    CASE(CUDA_ERROR_INVALID_ADDRESS_SPACE         );
    CASE(CUDA_ERROR_INVALID_PC                    );
    CASE(CUDA_ERROR_LAUNCH_FAILED                 );
    CASE(CUDA_ERROR_NOT_PERMITTED                 );
    CASE(CUDA_ERROR_NOT_SUPPORTED                 );
    CASE(CUDA_ERROR_UNKNOWN                       );
  };
# undef CASE
}


//
// global variables related to the CUDA state
//

static CUdevice     cu_device;
static char         device_name[256];
static CUcontext    cu_context;
static CUmodule     cu_module;
static CUfunction   cu_kernel;
static u32_t       *host_data;
static u32_t       *host_hash;
static CUdeviceptr  device_data;
static CUdeviceptr  device_hash;


//
// initialize the CUDA driver API interface
//

static void initialize_cuda(int device_number,const char *cubin_file_name,const char *kernel_name,u32_t data_size,u32_t hash_size)
{
  //
  // initialize the driver API interface
  //
  CU_CALL( cuInit , (0) );
  //
  // open the first CUDA device
  //
  CU_CALL( cuDeviceGet , (&cu_device,device_number) );
  //
  // get information about the CUDA device
  //
  CU_CALL( cuDeviceGetName , (device_name,(int)sizeof(device_name) - 1,cu_device) );
  printf("initialize_cuda: CUDA code running on a %s (device %d)\n",device_name,device_number);
  //
  // create a context
  //
  CU_CALL( cuCtxCreate , (&cu_context,CU_CTX_SCHED_YIELD,cu_device) ); // CU_CTX_SCHED_SPIN may be slightly faster
  CU_CALL( cuCtxSetCacheConfig , (CU_FUNC_CACHE_PREFER_L1) );
  //
  // load a precompiled module
  //
  CU_CALL( cuModuleLoad , (&cu_module,cubin_file_name) );
  //
  // get the kernel function pointer
  //
  CU_CALL( cuModuleGetFunction, (&cu_kernel,cu_module,kernel_name) );
  //
  // allocate host memory
  //
  if(data_size > 0u)
    CU_CALL( cuMemAllocHost , ((void **)&host_data,(size_t)data_size * sizeof(u32_t)) );
  else
    host_data = NULL;
  if(hash_size > 0u)
    CU_CALL( cuMemAllocHost , ((void **)&host_hash,(size_t)hash_size * sizeof(u32_t)) );
  else
    host_hash = NULL;
  //
  // allocate device memory
  //
  if(data_size > 0u)
    CU_CALL( cuMemAlloc , (&device_data,(size_t)data_size * sizeof(u32_t)) );
  if(hash_size > 0u)
    CU_CALL( cuMemAlloc , (&device_hash,(size_t)hash_size * sizeof(u32_t)) );
  //
  // done --- the call to cuStreamSynchronize() should catch any lingering errors
  //
  CU_CALL( cuStreamSynchronize , (0) );
}


//
// terminate the CUDA driver API interface
//

static void terminate_cuda(void)
{
  if(host_data != NULL)
  {
    CU_CALL( cuMemFreeHost , (host_data) );
    CU_CALL( cuMemFree , (device_data) );
  }
  if(host_hash != NULL)
  {
    CU_CALL( cuMemFreeHost , (host_hash) );
    CU_CALL( cuMemFree , (device_hash) );
  }
  CU_CALL( cuModuleUnload , (cu_module) );
  CU_CALL( cuCtxDestroy , (cu_context) );
}


#endif
#endif
