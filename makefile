#
# makefile for the first practical assignment (A1)
#

#
# CUDA installation directory --- /usr/local/cuda or $(CUDA_HOME)
#
CUDA_DIR = /usr/local/cuda

#
# OpenCL installation directory (for a NVidia graphics card, sama as CUDA)
#
OPENCL_DIR = $(CUDA_DIR)

#
# CUDA device architecture
#   GeForce GTX 1660 Ti --- sm_75
#   RTX A2000 Ada --------- sm_86
#   RTX A6000 Ada --------- sm_86
#   RTX 4070 -------------- sm_89
#
CUDA_ARCH = sm_86


#
# source code files
#
SRC       = deti_coins.c
H_FILES   = cpu_utilities.h search_utilities.h
H_FILES  += md5.h md5_test_data.h md5_cpu.h md5_cpu_avx.h md5_cpu_avx2.h md5_cpu_avx512.h md5_cpu_neon.h
H_FILES  += deti_coins_vault.h deti_coins_cpu_search.h deti_coins_cpu_avx_search.h deti_coins_cpu_avx2_search.h deti_coins_cpu_special_search.h deti_coins_cuda_search.h deti_coins_cpu_avx2_parallel_search.h deti_coins_cpu_avx512_search.h deti_coins_cpu_client_server_search.h
C_FILES   = cuda_driver_api_utilities.h md5_cuda.h 


#
# clean up
#
clean:
	rm -f a.out
	rm -f deti_coins_intel
	rm -f deti_coins_apple
	rm -f deti_coins_intel_cuda md5_cuda_kernel.cubin deti_coins_cuda_kernel_search.cubin
	rm -f deti_coins_wasm.html deti_coins_wasm.js deti_coins_wasm.wasm


#
# complie for webassembly
#
deti_coins_wasm: $(SRC) deti_coins_web_asembly_search.c
	emcc -Wall -O2 deti_coins_web_asembly_search.c -o deti_coins_wasm.html
#
# compile for Intel/AMD processors without CUDA
#
deti_coins_intel:	$(SRC) $(H_FILES)
	cc -Wall -O2 -fopenmp -mavx2 -DUSE_CUDA=0 $(SRC) -o deti_coins_intel


#
# compilation for Apple silicon without CUDA
#
deti_coins_apple:	$(SRC) $(H_FILES)
	cc -Wall -O2 -DUSE_CUDA=0 $(SRC) -o deti_coins_apple


#
# compile for Intel/AMD processors with CUDA
#
deti_coins_intel_cuda:	$(SRC) $(H_FILES) $(C_FILES) md5_cuda_kernel.cubin deti_coins_cuda_kernel_search.cubin
	cc -Wall -O2 -fopenmp -mavx2 -DUSE_CUDA=1 -I$(CUDA_DIR)/include $(SRC) -o deti_coins_intel_cuda -L$(CUDA_DIR)/lib64 -lcuda

md5_cuda_kernel.cubin:			md5.h md5_cuda_kernel.cu 
	nvcc -arch=$(CUDA_ARCH) --compiler-options -O2,-Wall -I$(CUDA_DIR)/include --cubin md5_cuda_kernel.cu -o md5_cuda_kernel.cubin

deti_coins_cuda_kernel_search.cubin:	md5.h deti_coins_cuda_kernel_search.cu
	nvcc -arch=$(CUDA_ARCH) --compiler-options -O2,-Wall -I$(CUDA_DIR)/include --cubin deti_coins_cuda_kernel_search.cu -o deti_coins_cuda_kernel_search.cubin
