//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// useful functions (cpu side)
//

#ifndef CPU_UTILITIES
#define CPU_UTILITIES


//
// macro to reserve the order of the bytes in a 32-bit integer
//

#if 1
# define SWAP(x)  __builtin_bswap32((unsigned int)(x))
#else
# define SWAP(x)  (((x) >> 24u) | ((x) << 24u) | (((x) >> 8u) & 0x0000FF00u) | (((x) << 8u) & 0x00FF0000u))
#endif


//
// function to reverse the order of the bytes of each word of a MD5 hash
//

static void hash_byte_reverse(u32_t hash[4])
{
  hash[0] = SWAP(hash[0]);
  hash[1] = SWAP(hash[1]);
  hash[2] = SWAP(hash[2]);
  hash[3] = SWAP(hash[3]);
}


//
// function to compute the DETI coin power (number of trailing zero bits of the byte-order reversed MD5 hash)
// WARNING: call hash_byte_reverse(hash) first!
//

static u32_t deti_coin_power(u32_t hash[4])
{
  int n;

  if(hash[3u] != 0u)
    n = __builtin_ctz(hash[3u]);
  else if(hash[2u] != 0u)
    n = 32u + __builtin_ctz(hash[2u]);
  else if(hash[1u] != 0u)
    n = 64u + __builtin_ctz(hash[1u]);
  else if(hash[0u] != 0u)
    n = 96u + __builtin_ctz(hash[0u]);
  else
    n = 128u;
  return n;
}


//
// measure elapsed and wall times n nano seconds (warning: Linux and macOS only)
//

static struct timespec measured_cpu_time[2],measured_wall_time[2];

static void time_measurement(void)
{
  measured_cpu_time[0] = measured_cpu_time[1];
  (void)clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&measured_cpu_time[1]);
  measured_wall_time[0] = measured_wall_time[1];
  (void)clock_gettime(CLOCK_MONOTONIC_RAW,&measured_wall_time[1]);
}

static double cpu_time_delta_ns(void)
{
  return 1.0e9 * ((double)measured_cpu_time[1].tv_sec  - (double)measured_cpu_time[0].tv_sec)
               + ((double)measured_cpu_time[1].tv_nsec - (double)measured_cpu_time[0].tv_nsec);
}

static double wall_time_delta_ns(void)
{
  return 1.0e9 * ((double)measured_wall_time[1].tv_sec  - (double)measured_wall_time[0].tv_sec)
               + ((double)measured_wall_time[1].tv_nsec - (double)measured_wall_time[0].tv_nsec);
}


//
// parse a time duration (a return of 0 means an error) --- assumes sizeof(long) is 8
//

static u32_t parse_time_duration(const char *time_duration)
{
  long days = -1l;
  long hours = -1l;
  long minutes = -1l;
  long seconds = -1l;
  long n;

  for(n = -1l;;time_duration++)
    switch(*time_duration)
    {
      default:
        return 0u;
      case '0' ... '9': // gcc or clang feature
        if(n < 0l)
          n = 0l;
        if(n >= (1l << 32))
          return 0u; // integer overflow
        n = 10l * n + (long)(*time_duration - '0');
        break;
# define CASE(letter,variable)  case letter: if(n < 0l || variable >= 0l) return 0u; variable = n; n = -1l; break;
      CASE('d',days)
      CASE('h',hours)
      CASE('m',minutes)
      CASE('s',seconds)
# undef CASE
      case '\0':
        if(n >= 0l) { if(seconds >= 0l) return 0u; seconds = n; }
        if(seconds < 0l) seconds = 0l;
        if(minutes < 0l) minutes = 0l;
        if(hours < 0l) hours = 0l;
        if(days < 0l) days = 0l;
        n = seconds + 60l * (minutes + 60l * (hours + 24l * days));
        return (n >= (1l << 32)) ? 0u : (u32_t)n;
    }
}

#endif
