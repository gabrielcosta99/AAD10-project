#include <omp.h> // Include OpenMP header

#ifndef DETI_COINS_CPU_AVX2_PARALLEL_SEARCH
#define DETI_COINS_CPU_AVX2_PARALLEL_SEARCH

static void deti_coins_cpu_avx2_parallel_search(u32_t n_random_words)
{
    int nLanes = 8;
    u32_t idx = 6;
    u64_t total_attempts = 0, total_coins = 0;

    // Parallelized outer loop
    #pragma omp parallel reduction(+:total_attempts, total_coins)
    {
        u32_t lane, i, v1, v2, n;
        u64_t n_attempts = 0, n_coins = 0;

        // Thread-local arrays
        u32_t coins_array[13 * nLanes] __attribute__((aligned(32)));
        u32_t hash_array[4 * nLanes] __attribute__((aligned(32)));

        int thread_id = omp_get_thread_num(); // Get thread ID
        for (lane = 0; lane < nLanes; lane++) {
            coins_array[0u * nLanes + lane] = 0x49544544; // "DETI"
            coins_array[1u * nLanes + lane] = 0x696f6320; // " coi"
            coins_array[2u * nLanes + lane] = 0x2020206E; // "n   "
            coins_array[12u * nLanes + lane] = 0x0A202020; // "   \n"

            for (i = 3u; i < 12u; i++) {
                coins_array[i * nLanes + lane] = 0x20202020;
            }

            // Ensure unique initialization for each thread
            coins_array[idx * nLanes + lane] = coins_array[idx * nLanes + lane] + lane + thread_id * 1000;
        }


        // Coin search loop
        while (stop_request == 0) {
            n_attempts += nLanes;

            // Compute MD5 hash
            md5_cpu_avx2((v8si *)coins_array, (v8si *)hash_array);

            for (lane = 0; lane < nLanes; lane++) {
                v1 = coins_array[idx * nLanes + lane];
                v2 = coins_array[(idx + 1) * nLanes + lane];
                u32_t hash[4];
                coin_t coin;

                for (i = 0u; i < 13u; i++) {
                    coin.coin_as_ints[i] = coins_array[i * nLanes + lane];
                }
                for (i = 0u; i < 4u; i++) {
                    hash[i] = hash_array[i * nLanes + lane];
                }

                // Byte-reverse hash
                hash_byte_reverse(hash);

                // Count trailing zeros
                n = deti_coin_power(hash);

                // Check if it's a DETI coin

                
                if (n >= 32u) {
#                   pragma omp critical
                    {
                    save_deti_coin(coin.coin_as_ints);
                    n_coins++;

                    }
                }
                
                // Update coin values
                v1 = next_value_to_try_ascii(v1 + nLanes - 1);
                if (v1 == 0x20202020) {
                    v2 = next_value_to_try_ascii(v2);
                    v1 += lane;
                }
                coins_array[idx * nLanes + lane] = v1;
                coins_array[(idx + 1) * nLanes + lane] = v2;
            }
        }

        // Accumulate thread-local counts
        total_attempts += n_attempts;
        total_coins += n_coins;
    }



    // Store the results
    STORE_DETI_COINS();

    // Print results
    printf("deti_coins_cpu_search: %lu DETI coin%s found in %lu attempt%s (expected %.2f coins)\n",
           total_coins, (total_coins == 1ul) ? "" : "s",
           total_attempts, (total_attempts == 1ul) ? "" : "s",
           (double)total_attempts / (double)(1ul << 32));
}

#endif
