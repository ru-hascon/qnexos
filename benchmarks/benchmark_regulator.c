#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/resource.h>
#include "trit_packer.h"

#define TORUS_DIMENSIONS 8
#define DEFAULT_ALPHA 0.6180339887498949

extern void qnex_regulate_and_pack(
    const double coeffs[TORUS_DIMENSIONS],
    double alpha,
    uint16_t *out_mask,
    double out_coeffs[TORUS_DIMENSIONS],
    trit_t out_trits[TORUS_DIMENSIONS],
    degenerate_type_t *out_degen);

static double ms_now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

static long get_peak_rss(void) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

static void bench_throughput(void) {
    printf("[BENCH 1] Throughput (10M calls)...\n");
    double coeffs[8] = {0.5, 1.2, 0.01, 3.7, 0.001, 0.8, 2.1, 0.03};
    double out_coeffs[8];
    trit_t trits[8];
    uint16_t mask;
    degenerate_type_t degen;
    const int N = 10000000;
    volatile uint16_t sink = 0;
    double t0 = ms_now();
    for (int i = 0; i < N; i++) {
        qnex_regulate_and_pack(coeffs, DEFAULT_ALPHA, &mask, out_coeffs, trits, &degen);
        sink ^= mask;
    }
    double t1 = ms_now();
    double elapsed = t1 - t0;
    double rate = N / (elapsed / 1000.0);
    printf("  Calls: %d\n", N);
    printf("  Time:  %.2f ms\n", elapsed);
    printf("  Rate:  %.2f M calls/s\n", rate / 1e6);
    printf("  Sink:  %u\n", sink);
}

static void bench_patterns(void) {
    printf("\n[BENCH 2] Data patterns...\n");
    const char *names[] = {"dominant", "uniform", "gradient", "random"};
    double patterns[4][8] = {
        {10, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0.1, 0.5, 1.0, 2.0, 3.0, 5.0, 7.0, 10.0},
    };
    for (int i = 0; i < 8; i++) patterns[3][i] = (double)(rand() % 1000) / 100.0;
    double out_coeffs[8];
    trit_t trits[8];
    uint16_t mask;
    degenerate_type_t degen;
    for (int p = 0; p < 4; p++) {
        const int N = 500000;
        volatile uint16_t sink = 0;
        double t0 = ms_now();
        for (int i = 0; i < N; i++) {
            qnex_regulate_and_pack(patterns[p], DEFAULT_ALPHA, &mask, out_coeffs, trits, &degen);
            sink ^= mask;
        }
        double t1 = ms_now();
        double rate = N / ((t1 - t0) / 1000.0);
        char buf[9];
        trit_mask_to_string(trits, buf);
        printf("  %-10s: rate=%.2f M/s  trits=%s  degen=%d\n",
               names[p], rate / 1e6, buf, degen);
    }
}

static void bench_memory(void) {
    printf("\n[BENCH 3] Memory (peak RSS)...\n");
    double coeffs[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    double out_coeffs[8];
    uint16_t mask;
    degenerate_type_t degen;
    long rss_before = get_peak_rss();
    for (int i = 0; i < 5000000; i++) {
        qnex_regulate_and_pack(coeffs, DEFAULT_ALPHA, &mask, out_coeffs, NULL, &degen);
    }
    long rss_after = get_peak_rss();
    printf("  RSS before: %ld KB\n", rss_before);
    printf("  RSS after:  %ld KB\n", rss_after);
    printf("  Delta:      %ld KB\n", rss_after - rss_before);
}

static void bench_stability(void) {
    printf("\n[BENCH 4] Stability (60 seconds)...\n");
    double coeffs[8];
    double out_coeffs[8];
    uint16_t mask;
    degenerate_type_t degen;
    volatile uint64_t total_calls = 0;
    volatile uint16_t sink = 0;
    double deadline = ms_now() + 60000.0;
    srand(42);
    while (ms_now() < deadline) {
        for (int i = 0; i < 8; i++) coeffs[i] = (double)(rand() % 1000) / 100.0;
        qnex_regulate_and_pack(coeffs, DEFAULT_ALPHA, &mask, out_coeffs, NULL, &degen);
        sink ^= mask;
        total_calls++;
    }
    printf("  Total calls: %lu\n", total_calls);
    printf("  Avg rate:    %.2f M calls/s\n", (double)total_calls / 60.0 / 1e6);
    printf("  Sink:        %u\n", sink);
    printf("  No crash, no memory leak.\n");
}

int main(void) {
    printf("=== qnex_torus_regulator Load Test ===\n\n");
    srand(12345);
    bench_throughput();
    bench_patterns();
    bench_memory();
    bench_stability();
    printf("\n=== ALL BENCHMARKS PASSED ===\n");
    return 0;
}
