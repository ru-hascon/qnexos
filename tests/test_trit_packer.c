#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "trit_packer.h"

static void test_exhaustive(void) {
    printf("[TEST 1] Exhaustive round-trip: 0..6560...\n");
    trit_t original[8], restored[8];
    uint16_t packed;
    int errors = 0;
    for (uint16_t i = 0; i <= TRIT_PACKED_MAX; i++) {
        uint16_t temp = i;
        for (int k = 0; k < TRIT_COUNT; k++) {
            original[k] = (trit_t)((int16_t)(temp % 3) - 1);
            temp /= 3;
        }
        if (trit_pack(original, &packed) != 0) { errors++; continue; }
        if (packed != i)                       { errors++; continue; }
        if (trit_is_degenerate(packed))        { errors++; continue; }
        if (trit_unpack(packed, restored) != 0) { errors++; continue; }
        for (int k = 0; k < TRIT_COUNT; k++) {
            if (restored[k] != original[k]) { errors++; break; }
        }
    }
    printf("  Result: PASS (%d errors)\n", errors);
    assert(errors == 0);
}

static void test_degenerate(void) {
    printf("[TEST 2] Degenerate codes: 6561..8191...\n");
    trit_t dummy[8];
    int errors = 0;
    for (uint16_t i = TRIT_DEGENERATE_BASE; i <= TRIT_PACKED_RESERVED; i++) {
        if (!trit_is_degenerate(i))      errors++;
        if (trit_unpack(i, dummy) != -1) errors++;
    }
    printf("  Result: PASS (%d errors)\n", errors);
    assert(errors == 0);
}

static void test_reserved_bit(void) {
    printf("[TEST 3] Reserved bits (13) and parity bit (14)...\n");
    trit_t dummy[8];
    
    /* Бит 13 — reserved (должен быть 0) */
    int rc = trit_unpack(0x2000 | TRIT_PACKED_MAX, dummy);
    printf("  Bit 13 (reserved): rc=%d (expected -2) -> %s\n", rc, rc == -2 ? "PASS" : "FAIL");
    assert(rc == -2);
    
    /* Бит 14 — parity. Если он установлен, но данные не совпадают — это ошибка parity */
    rc = trit_unpack(0x4000 | TRIT_PACKED_MAX, dummy);
    printf("  Bit 14 (parity):   rc=%d (expected -1) -> %s\n", rc, rc == -1 ? "PASS" : "FAIL");
    assert(rc == -1);
}

static void test_parity(void) {
    printf("[TEST 4] Parity round-trip: 0..6560...\n");
    int errors = 0;
    for (uint16_t i = 0; i <= TRIT_PACKED_MAX; i++) {
        uint16_t mask = topo_form_protected_mask(i);
        if (!topo_check_parity(mask)) errors++;
        if (topo_get_version(mask) != TOPO_PROTOCOL_VERSION) errors++;
    }
    printf("  Result: PASS (%d errors)\n", errors);
    assert(errors == 0);
}

static void test_hamming_distance(void) {
    printf("[TEST 5] Hamming distance...\n");
    trit_t all_neg[8] = {TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG};
    trit_t all_pos[8] = {TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS};
    trit_t half[8]    = {TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS};
    assert(trit_hamming_distance(all_neg, all_pos) == 8);
    assert(trit_hamming_distance(all_neg, half) == 4);
    assert(trit_hamming_distance(all_neg, all_neg) == 0);
    printf("  Result: PASS\n");
}

static void test_majority_vote(void) {
    printf("[TEST 6] Majority vote (5 masks)...\n");
    trit_t masks[5][8] = {
        {TRIT_POS, TRIT_POS, TRIT_POS, TRIT_ZRO, TRIT_ZRO, TRIT_NEG, TRIT_NEG, TRIT_NEG},
        {TRIT_POS, TRIT_POS, TRIT_ZRO, TRIT_ZRO, TRIT_ZRO, TRIT_NEG, TRIT_NEG, TRIT_NEG},
        {TRIT_POS, TRIT_ZRO, TRIT_ZRO, TRIT_ZRO, TRIT_ZRO, TRIT_NEG, TRIT_NEG, TRIT_POS},
        {TRIT_ZRO, TRIT_ZRO, TRIT_ZRO, TRIT_ZRO, TRIT_ZRO, TRIT_ZRO, TRIT_ZRO, TRIT_ZRO},
        {TRIT_POS, TRIT_POS, TRIT_POS, TRIT_POS, TRIT_ZRO, TRIT_NEG, TRIT_NEG, TRIT_NEG},
    };
    trit_t result[8];
    trit_majority_vote(masks, 5, result);
    assert(result[0] == TRIT_POS);
    assert(result[1] == TRIT_POS);
    assert(result[2] == TRIT_ZRO);
    assert(result[3] == TRIT_ZRO);
    assert(result[4] == TRIT_ZRO);
    assert(result[5] == TRIT_NEG);
    assert(result[6] == TRIT_NEG);
    assert(result[7] == TRIT_NEG);
    printf("  Result: PASS\n");
}

static void test_entropy(void) {
    printf("[TEST 7] Entropy...\n");
    trit_t all_neg[8] = {TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG};
    trit_t all_zro[8] = {TRIT_ZRO,TRIT_ZRO,TRIT_ZRO,TRIT_ZRO,TRIT_ZRO,TRIT_ZRO,TRIT_ZRO,TRIT_ZRO};
    trit_t mixed[8]   = {TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_ZRO,TRIT_ZRO,TRIT_POS,TRIT_POS,TRIT_POS};
    double h_neg = trit_entropy_bits(all_neg);
    double h_zro = trit_entropy_bits(all_zro);
    double h_mix = trit_entropy_bits(mixed);
    assert(h_neg < 0.001);
    assert(h_zro < 0.001);
    assert(h_mix > 12.0 && h_mix < 13.0);
    assert(fabs(h_neg - trit_entropy_bits_fast(all_neg)) < 0.001);
    assert(fabs(h_mix - trit_entropy_bits_fast(mixed)) < 0.001);
    printf("  Result: PASS\n");
}

static void test_string(void) {
    printf("[TEST 8] String representation...\n");
    trit_t trits[8] = {TRIT_POS, TRIT_ZRO, TRIT_NEG, TRIT_POS, TRIT_ZRO, TRIT_NEG, TRIT_POS, TRIT_ZRO};
    char str[9];
    trit_mask_to_string(trits, str);
    assert(strcmp(str, "+0-+0-+0") == 0);
    printf("  Result: PASS\n");
}

static void test_serialization(void) {
    printf("[TEST 9] Network serialization...\n");
    int errors = 0;
    for (uint16_t i = 0; i <= TRIT_PACKED_MAX; i++) {
        uint8_t net[2];
        trit_mask_to_network(i, net);
        if (trit_mask_from_network(net) != i) errors++;
    }
    printf("  Result: PASS (%d errors)\n", errors);
    assert(errors == 0);
}

static void test_compare(void) {
    printf("[TEST 10] Trit compare...\n");
    trit_t a[8] = {TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG};
    trit_t b[8] = {TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS};
    assert(trit_compare(a, b) < 0);
    assert(trit_compare(b, a) > 0);
    assert(trit_compare(a, a) == 0);
    printf("  Result: PASS\n");
}

static void test_consistency(void) {
    printf("[TEST 11] Consistency with active_domains_map...\n");
    trit_t trits[8] = {TRIT_POS, TRIT_NEG, TRIT_POS, TRIT_ZRO, TRIT_ZRO, TRIT_NEG, TRIT_NEG, TRIT_POS};
    uint8_t map = 0;
    for (int i = 0; i < 8; i++) {
        if (trits[i] == TRIT_POS) map |= (1 << i);
    }
    assert(trit_validate_consistency(trits, map) == 0);
    map |= (1 << 1); 
    assert(trit_validate_consistency(trits, map) == -1);
    printf("  Result: PASS\n");
}

static void test_estimate_size(void) {
    printf("[TEST 12] Estimate transmission size...\n");
    trit_t all_active[8] = {TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS,TRIT_POS};
    trit_t all_collapsed[8] = {TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG,TRIT_NEG};
    assert(topo_estimate_transmission_size(all_active, DEGEN_NONE) == 48 + 8 * 16);
    assert(topo_estimate_transmission_size(all_collapsed, DEGEN_NONE) == 48);
    assert(topo_estimate_transmission_size(all_collapsed, DEGEN_ZERO) == 48);
    printf("  Result: PASS\n");
}

int main(void) {
    printf("=== TritPacker Exhaustive Test Suite ===\n\n");
    test_exhaustive();
    test_degenerate();
    test_reserved_bit();
    test_parity();
    test_hamming_distance();
    test_majority_vote();
    test_entropy();
    test_string();
    test_serialization();
    test_compare();
    test_consistency();
    test_estimate_size();
    printf("\n=== ALL 12 TESTS PASSED ===\n");
    return 0;
}
