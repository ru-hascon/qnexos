# QNEX OS v1.0 «Nexus» — Architectural Decisions

**QNEX** = **Q**uantum **NEX**us Operating System
(Квантовая Узловая Операционная Система — мост между классическими и квантовыми вычислениями)

## AD-001: 8 Domains (E₈ Lattice)
- **Status:** APPROVED
- **Domains:** D0 (RT-Core + INIT), D1 (QMEM), D2 (TELE), D3 (TORIC), D4 (API), D5 (NET), D6 (ADAPT/AI), D7 (AUDIT)
- **Rationale:** Optimal packing density (E₈), fault isolation, full lifecycle coverage

## AD-002: Logical Teleportation (Transfer-of-Ownership)
- **Status:** APPROVED
- **Rationale:** No data duplication, nanosecond latency
- **Constraint:** Classical Bennett protocol only. No FTL claims.

## AD-003: Ternary Logic (TRIT_POS/ZRO/NEG)
- **Status:** APPROVED
- **Rationale:** 15-bit savings in uncertainty zone
- **Constraint:** α = φ⁻¹ ≈ 0.618, Horner's scheme, DELTA_MULTIPLIER = 0.2

## AD-004: Torus Collapse via Discrete Ricci Flow
- **Status:** APPROVED
- **Rationale:** Direct application of Poincaré conjecture (Perelman, 2003)
- **Constraint:** Async computation (D3), O(1) application (D0)

## AD-005: Zero Matrices for Degenerate Blocks
- **Status:** APPROVED
- **Rationale:** 512x compression for empty blocks

## AD-006: 1024-Byte Blocks
- **Status:** APPROVED
- **Rationale:** Power of 2, AVX-512 alignment, MTU compatibility

## AD-007: Surface Code
- **Status:** APPROVED
- **Rationale:** Proven (Google/IBM), replaces unproven zeta-encoding

## AD-008: AI Domain (ADAPT) — Self-Evolution
- **Status:** APPROVED
- **Rationale:** Continuous optimization without human intervention
- **Constraint:** AI proposes → AUDIT validates → ADAPT applies

## AD-009: Persistence Model (DSPP)
- **Status:** APPROVED
- **Free tier:** NVMe + WAL (64KB, double buffering)
- **Paid tier:** +PQC encryption + quantum accelerators

## AD-010: Physical Limits Compliance
- **Status:** APPROVED
- **Shannon limit:** 12.68 bits theoretical, 13 bits actual
- **Landauer limit:** 2.9×10⁻²¹ J/bit theoretical

## AD-011: Adaptive Quantum Error Correction (AQEC)
- **Status:** APPROVED
- **Rationale:** Strategy pattern for different noise profiles
