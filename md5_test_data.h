//
// Tomás Oliveira e Silva,  October 2024
//
// Arquiteturas de Alto Desempenho 2024/2025
//
// MD5 custom message digest test data
//
// host_md5_test_data[] ---------- the N_MESSAGES messages ---------------- initialized with random data by make_random_md5_test_data()
// host_md5_test_hash[] ---------- their respective MD5 message digests --- computed by test_md5_cpu()
//

#ifndef MD5_TEST_DATA
#define MD5_TEST_DATA

#ifndef N_MESSAGES
# define N_MESSAGES (1u << 20)
#endif

static u32_t host_md5_test_data[N_MESSAGES * 13u]; // interpreted as [N_MESSAGES][13u] --- 13 consecutive integers for each message
static u32_t host_md5_test_hash[N_MESSAGES *  4u]; // interpreted as [N_MESSAGES][ 4u] --- 4 consecutive integers for each MD5 hash

static void make_random_md5_test_data(void)
{
  u32_t i;

  srand(time(NULL));
  for(i = 0u;i < N_MESSAGES * 13u;i++)
    host_md5_test_data[i] = ((u32_t)random() & 0xFFFF) | ((u32_t)random() << 16u); // the values for i % 16 == 13, 14, or 15 will be ignored
}

#endif
