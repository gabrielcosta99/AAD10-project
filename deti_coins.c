//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// DETI coins main program (possible solution)
//

#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>


#if defined(__GNUC__) && __BYTE__ORDER__ != __LITTLE_ENDIAN__
# error "this code requires a little-endian processor"
#endif

#ifndef USE_CUDA
# define USE_CUDA 0
#endif


//
// unsigned integer data types and some useful functions (in cpu_utilities.h)
//

typedef unsigned char u08_t;
typedef unsigned  int u32_t;
typedef unsigned long u64_t;

#include "cpu_utilities.h"


//
// MD5 hash implementations and respective tests
//
// Intel Core i7-5500U CPU @ 2.40GHz, turbo boosted to 3.00GHz
//   time per md5 hash ( cpu): 122.627ns 122.626ns
//   time per md5 hash ( avx):  45.529ns  45.529ns
//   time per md5 hash (avx2):  22.695ns  22.695ns
//
// Intel(R) Core(TM) i5-8400T CPU @ 1.70GHz, turbo boosted to 3.30GHz
//   time per md5 hash ( cpu): 102.579ns 102.579ns
//   time per md5 hash ( avx):  37.310ns  37.310ns
//   time per md5 hash (avx2):  18.675ns  18.675ns
//
// AMD Ryzen 7 4800HS with Radeon Graphics (2.90GHz)
//   time per md5 hash ( cpu):  75.165ns  75.165ns
//   time per md5 hash ( avx):  31.903ns  31.902ns
//   time per md5 hash (avx2):  15.962ns  15.962ns
//
// Mac Mini M2 high-performance "Avalanche" core (3.49GHz)
//   time per md5 hash ( cpu):  84.959ns  85.057ns
//   time per md5 hash (neon):  40.114ns  40.144ns
// Mac Mini M2 energy-efficient "Blizzard" core (2.42GHz)
//   time per md5 hash ( cpu): 144.973ns 145.190ns
//   time per md5 hash (neon):  76.488ns  76.585ns
//

#include "md5.h"
#include "md5_test_data.h"
#include "md5_cpu.h"
#include "md5_cpu_avx.h"
#include "md5_cpu_avx2.h"
#include "md5_cpu_neon.h"
#if USE_CUDA > 0
# include "cuda_driver_api_utilities.h"
# include "md5_cuda.h"
#endif

static void all_md5_tests(void)
{
  //
  // make random test data
  //
  make_random_md5_test_data();
  //
  // any: md5_cpu() tests
  //
  test_md5_cpu();
  //
  // intel/amd: md5_cpu_avx() tests --- comparison with the hash data computed by test_cpu_md5()
  //
#ifdef MD5_CPU_AVX
  test_md5_cpu_avx();
#endif
  //
  // intel/amd: md5_cpu_avx2() tests --- comparison with the hash data computed by test_cpu_md5()
  //
#ifdef MD5_CPU_AVX2
  test_md5_cpu_avx2();
#endif
  //
  // arm: md5_cpu_neon() tests --- comparison with the hash data computed by test_cpu_md5()
  //
#ifdef MD5_CPU_NEON
  test_md5_cpu_neon();
#endif
  //
  // cuda: md5_cuda() tests --- comparison with the hash data computed by test_cpu_md5()
  //
#ifdef MD5_CUDA
  test_md5_cuda();
#endif
}


//
// saving are reporting DETI coins
//

#include "deti_coins_vault.h"


//
// search for DETI coins
//

static volatile int stop_request;

static void alarm_signal_handler(int dummy)
{
  stop_request = 1;
}

#include "deti_coins_cpu_search.h"
//#include "deti_coins_cpu_special_search.h"

//#include "search_utilities.h"
//#ifdef MD5_CPU_AVX
//# include "deti_coins_cpu_avx_search.h"
//#endif
//#ifdef MD5_CPU_AVX2
//# include "deti_coins_cpu_avx2_search.h"
//#endif
//#ifdef MD5_CPU_NEON
//# include "deti_coins_cpu_neon_search.h"
//#endif
//#if USE_CUDA > 0
//# include "deti_coins_cuda_search.h"
//#endif


//
// main program
//

int main(int argc,char **argv)
{
  u32_t seconds,n_random_words;

  //
  // correctness tests (-t command line option)
  //
  if(argc == 2 && argv[1][0] == '-' && argv[1][1] == 't')
  {
#ifdef SEARCH_UTILITIES
    test_next_value_to_try_ascii(); // this will help warming up (turbo boost) the processor!
#endif
    all_md5_tests();
    return 0;
  }
  //
  // search for DETI coins (-s command line option)
  //
  if((argc >= 2 && argc <= 4) && argv[1][0] == '-' && argv[1][1] == 's')
  {
    srandom((unsigned int)time(NULL));
    seconds = (argc > 2) ? parse_time_duration(argv[2]) : 1800u;
    if(seconds == 0u)
    {
      fprintf(stderr,"main: bad number of seconds --- format [Nd][Nh][Nm][N[s]], where each N is a number and [] means whats inside it is optional\n");
      exit(1);
    }
    if(seconds < 120u)
      seconds = 120u; // at least 2 minutes
    if(seconds > 7200u)
      seconds = 7200u; // at most 2 hours
    n_random_words = (argc > 3) ? (u32_t)atol(argv[3]) : 1u;
    if(n_random_words > 9u)
      n_random_words = 9u;
    stop_request = 0;
    (void)signal(SIGALRM,alarm_signal_handler);
    (void)alarm((unsigned int)seconds);
    switch(argv[1][2])
    {
      default:
        fprintf(stderr,"unknown -s option\n");
        exit(1);
      case '\0':
      case '0':
        printf("searching for %u seconds using deti_coins_cpu_search()\n",seconds);
        fflush(stdout);
        deti_coins_cpu_search();
        break;
#ifdef DETI_COINS_CPU_AVX_SEARCH
      case '1':
        printf("searching for %u seconds using deti_coins_cpu_avx_search()\n",seconds);
        fflush(stdout);
        deti_coins_cpu_avx_search(n_random_words);
        break;
#endif
#ifdef DETI_COINS_CPU_AVX2_SEARCH
      case '2':
        printf("searching for %u seconds using deti_coins_cpu_avx2_search()\n",seconds);
        fflush(stdout);
        deti_coins_cpu_avx2_search(n_random_words);
        break;
#endif
#ifdef DETI_COINS_CPU_NEON_SEARCH
      case '3':
        printf("searching for %u seconds using deti_coins_cpu_neon_search()\n",seconds);
        fflush(stdout);
        deti_coins_cpu_neon_search(n_random_words);
        break;
#endif
#ifdef DETI_COINS_CUDA_SEARCH
      case '4':
        printf("searching for %u seconds using deti_coins_cuda_search()\n",seconds);
        fflush(stdout);
        deti_coins_cuda_search(n_random_words);
        break;
#endif
#ifdef DETI_COINS_CPU_SPECIAL_SEARCH
      case '9':
        printf("searching for %u seconds using deti_coins_cpu_special_search()\n",seconds);
        fflush(stdout);
        deti_coins_cpu_special_search();
        break;
#endif
    }
    return 0;
  }
  fprintf(stderr,"usage: %s -t                               # MD5 hash tests\n",argv[0]);
  fprintf(stderr,"       %s -s0 [seconds] [ignored]          # search for DETI coins using md5_cpu()\n",argv[0]);
#ifdef DETI_COINS_CPU_AVX_SEARCH
  fprintf(stderr,"       %s -s1 [seconds] [n_random_words]   # search for DETI coins using md5_cpu_avx()\n",argv[0]);
#endif
#ifdef DETI_COINS_CPU_AVX2_SEARCH
  fprintf(stderr,"       %s -s2 [seconds] [n_random_words]   # search for DETI coins using md5_cpu_avx2()\n",argv[0]);
#endif
#ifdef DETI_COINS_CPU_NEON_SEARCH
  fprintf(stderr,"       %s -s3 [seconds] [n_random_words]   # search for DETI coins using md5_cpu_neon()\n",argv[0]);
#endif
#ifdef DETI_COINS_CUDA_SEARCH
  fprintf(stderr,"       %s -s4 [seconds] [n_random_words]   # search for DETI coins using CUDA\n",argv[0]);
#endif
#ifdef DETI_COINS_CPU_SPECIAL_SEARCH
  fprintf(stderr,"       %s -s9 [seconds] [ignored]          # special search for DETI coins using md5_cpu()\n",argv[0]);
#endif
  fprintf(stderr,"                                           #   seconds is the amount of time spent in the search\n");
  fprintf(stderr,"                                           #   n_random_words is the number of 4-byte words to use\n");
  return 1;
}
