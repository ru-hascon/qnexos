/**
 * @file benchmark_cpu.c
 * @brief Бенчмарк производительности TritPacker на CPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "trit_packer.h"
#include "topo_gf3.h"

#define BENCHMARK_ITERATIONS 10000000  // 10 миллионов итераций
#define ARRAY_SIZE 1000000              // 1 миллион элементов

// Структура для хранения результатов бенчмарка
typedef struct {
    const char *name;
    double time_ms;
    double operations_per_sec;
    double bytes_per_sec;
} benchmark_result_t;

// Измерение времени с высокой точностью
double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

// Бенчмарк упаковки тритов
benchmark_result_t benchmark_pack() {
    trit_t trits[TRIT_COUNT] = {TRIT_POS, TRIT_NEG, TRIT_ZRO, TRIT_POS, 
                                 TRIT_NEG, TRIT_ZRO, TRIT_POS, TRIT_NEG};
    uint16_t packed;
    
    double start = get_time_ms();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        trit_pack(trits, &packed);
        // Изменяем триты для предотвращения оптимизации
        trits[i % TRIT_COUNT] = (trits[i % TRIT_COUNT] + 1) % 3 - 1;
    }
    double end = get_time_ms();
    
    benchmark_result_t result;
    result.name = "trit_pack";
    result.time_ms = end - start;
    result.operations_per_sec = BENCHMARK_ITERATIONS / (result.time_ms / 1000.0);
    result.bytes_per_sec = (BENCHMARK_ITERATIONS * sizeof(trit_t) * TRIT_COUNT) / (result.time_ms / 1000.0);
    
    return result;
}

// Бенчмарк распаковки тритов
benchmark_result_t benchmark_unpack() {
    uint16_t packed = 3280;  // Среднее значение
    trit_t trits[TRIT_COUNT];
    
    double start = get_time_ms();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        trit_unpack(packed, trits);
        packed = (packed + 1) % TRIT_PACKED_MAX;
    }
    double end = get_time_ms();
    
    benchmark_result_t result;
    result.name = "trit_unpack";
    result.time_ms = end - start;
    result.operations_per_sec = BENCHMARK_ITERATIONS / (result.time_ms / 1000.0);
    result.bytes_per_sec = (BENCHMARK_ITERATIONS * sizeof(trit_t) * TRIT_COUNT) / (result.time_ms / 1000.0);
    
    return result;
}

// Бенчмарк вычисления энтропии
benchmark_result_t benchmark_entropy() {
    trit_t trits[TRIT_COUNT] = {TRIT_POS, TRIT_NEG, TRIT_ZRO, TRIT_POS, 
                                 TRIT_NEG, TRIT_ZRO, TRIT_POS, TRIT_NEG};
    double entropy;
    
    double start = get_time_ms();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        entropy = trit_entropy_bits(trits);
        trits[i % TRIT_COUNT] = (trits[i % TRIT_COUNT] + 1) % 3 - 1;
    }
    double end = get_time_ms();
    
    benchmark_result_t result;
    result.name = "trit_entropy_bits";
    result.time_ms = end - start;
    result.operations_per_sec = BENCHMARK_ITERATIONS / (result.time_ms / 1000.0);
    result.bytes_per_sec = 0;
    
    return result;
}

// Бенчмарк быстрой энтропии (LUT)
benchmark_result_t benchmark_entropy_fast() {
    trit_t trits[TRIT_COUNT] = {TRIT_POS, TRIT_NEG, TRIT_ZRO, TRIT_POS, 
                                 TRIT_NEG, TRIT_ZRO, TRIT_POS, TRIT_NEG};
    double entropy;
    
    double start = get_time_ms();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        entropy = trit_entropy_bits_fast(trits);
        trits[i % TRIT_COUNT] = (trits[i % TRIT_COUNT] + 1) % 3 - 1;
    }
    double end = get_time_ms();
    
    benchmark_result_t result;
    result.name = "trit_entropy_bits_fast";
    result.time_ms = end - start;
    result.operations_per_sec = BENCHMARK_ITERATIONS / (result.time_ms / 1000.0);
    result.bytes_per_sec = 0;
    
    return result;
}

// Бенчмарк мажоритарного голосования
benchmark_result_t benchmark_majority_vote() {
    trit_t masks[5][TRIT_COUNT];
    trit_t result[TRIT_COUNT];
    
    // Инициализация масок
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < TRIT_COUNT; j++) {
            masks[i][j] = (i + j) % 3 - 1;
        }
    }
    
    double start = get_time_ms();
    for (int i = 0; i < BENCHMARK_ITERATIONS / 10; i++) {
        trit_majority_vote(masks, 5, result);
    }
    double end = get_time_ms();
    
    benchmark_result_t res;
    res.name = "trit_majority_vote";
    res.time_ms = end - start;
    res.operations_per_sec = (BENCHMARK_ITERATIONS / 10) / (res.time_ms / 1000.0);
    res.bytes_per_sec = 0;
    
    return res;
}

// Бенчмарк обработки массива
benchmark_result_t benchmark_array_processing() {
    trit_t *trits_array = malloc(ARRAY_SIZE * TRIT_COUNT * sizeof(trit_t));
    uint16_t *packed_array = malloc(ARRAY_SIZE * sizeof(uint16_t));
    
    // Инициализация случайными данными
    srand(42);
    for (int i = 0; i < ARRAY_SIZE * TRIT_COUNT; i++) {
        trits_array[i] = (rand() % 3) - 1;
    }
    
    double start = get_time_ms();
    for (int i = 0; i < ARRAY_SIZE; i++) {
        trit_pack(&trits_array[i * TRIT_COUNT], &packed_array[i]);
    }
    double end = get_time_ms();
    
    benchmark_result_t result;
    result.name = "array_pack_1M";
    result.time_ms = end - start;
    result.operations_per_sec = ARRAY_SIZE / (result.time_ms / 1000.0);
    result.bytes_per_sec = (ARRAY_SIZE * TRIT_COUNT * sizeof(trit_t)) / (result.time_ms / 1000.0);
    
    free(trits_array);
    free(packed_array);
    
    return result;
}

// Бенчмарк GF(3) операций
benchmark_result_t benchmark_gf3_operations() {
    gf3_t a = 2, b = 1;
    gf3_t result;
    
    double start = get_time_ms();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        result = gf3_add(a, b);
        result = gf3_mul(result, a);
        result = gf3_sub(result, b);
        a = (a + 1) % 3;
        b = (b + 1) % 3;
    }
    double end = get_time_ms();
    
    benchmark_result_t res;
    res.name = "gf3_operations";
    res.time_ms = end - start;
    res.operations_per_sec = (BENCHMARK_ITERATIONS * 3) / (res.time_ms / 1000.0);
    res.bytes_per_sec = 0;
    
    return res;
}

// Измерение использования памяти
void measure_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    printf("\n=== Использование памяти ===\n");
    printf("Максимальный RSS: %ld KB (%.2f MB)\n", 
           usage.ru_maxrss, usage.ru_maxrss / 1024.0);
    printf("Мягкие page faults: %ld\n", usage.ru_minflt);
    printf("Жёсткие page faults: %ld\n", usage.ru_majflt);
}

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  TritPacker v1.1.0 — CPU Benchmark Suite                  ║\n");
    printf("║  Iterations: %d, Array Size: %d                       ║\n", 
           BENCHMARK_ITERATIONS, ARRAY_SIZE);
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    benchmark_result_t results[7];
    int num_benchmarks = 0;
    
    // Запуск всех бенчмарков
    printf("Запуск бенчмарков...\n\n");
    
    results[num_benchmarks++] = benchmark_pack();
    printf("✓ %s завершён\n", results[num_benchmarks-1].name);
    
    results[num_benchmarks++] = benchmark_unpack();
    printf("✓ %s завершён\n", results[num_benchmarks-1].name);
    
    results[num_benchmarks++] = benchmark_entropy();
    printf("✓ %s завершён\n", results[num_benchmarks-1].name);
    
    results[num_benchmarks++] = benchmark_entropy_fast();
    printf("✓ %s завершён\n", results[num_benchmarks-1].name);
    
    results[num_benchmarks++] = benchmark_majority_vote();
    printf("✓ %s завершён\n", results[num_benchmarks-1].name);
    
    results[num_benchmarks++] = benchmark_array_processing();
    printf("✓ %s завершён\n", results[num_benchmarks-1].name);
    
    results[num_benchmarks++] = benchmark_gf3_operations();
    printf("✓ %s завершён\n", results[num_benchmarks-1].name);
    
    // Вывод результатов
    printf("\n╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           РЕЗУЛЬТАТЫ БЕНЧМАРКОВ                            ║\n");
    printf("╠══════════════════════════════╦═══════════╦══════════════╦═══════════════════╣\n");
    printf("║ Функция                      ║ Время, мс ║ Оп/сек       ║ Байт/сек          ║\n");
    printf("╠══════════════════════════════╬═══════════╬══════════════╬═══════════════════╣\n");
    
    for (int i = 0; i < num_benchmarks; i++) {
        printf("║ %-28s ║ %9.2f ║ %12.2f ║ %17.2f ║\n",
               results[i].name,
               results[i].time_ms,
               results[i].operations_per_sec,
               results[i].bytes_per_sec);
    }
    
    printf("╚══════════════════════════════╩═══════════╩══════════════╩═══════════════════╝\n");
    
    // Измерение памяти
    measure_memory_usage();
    
    // Системная информация
    printf("\n=== Системная информация ===\n");
    printf("Компиляция: -O3 -march=native -mtune=native\n");
    printf("Архитектура: x86_64\n");
    
    return 0;
}
