// gate.h
#pragma once

#include <core/types.h>

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

// Packed U64 fields are opcode,q0,q1,q2, each 16 bits wide.
// Symmetric operands and Toffoli controls are stored in canonical order.
namespace cnot {

inline constexpr U16 invalid  = 0;
inline constexpr U16 x        = 1;
inline constexpr U16 z        = 2;
inline constexpr U16 cnot     = 3;
inline constexpr U16 cz       = 4;
inline constexpr U16 toffoli  = 5;
inline constexpr U16 ccz      = 6;
inline constexpr U16 s        = 7;
inline constexpr U16 sdg      = 8;
inline constexpr U16 no_qubit = 0xffff;

inline constexpr U64 pack(
    U16 op, U16 q0 = no_qubit, U16 q1 = no_qubit,
    U16 q2 = no_qubit) noexcept {
    return static_cast<U64>(op) |
           (static_cast<U64>(q0) << 16) |
           (static_cast<U64>(q1) << 32) |
           (static_cast<U64>(q2) << 48);
}

inline constexpr U16 opcode(U64 gate) noexcept {
    return static_cast<U16>(gate);
}

inline constexpr U16 qubit(U64 gate, unsigned index) noexcept {
    return index < 3
        ? static_cast<U16>(gate >> (16 * (index + 1)))
        : no_qubit;
}

inline constexpr U64 make_x(U16 q) noexcept {
    return pack(x, q);
}

inline constexpr U64 make_z(U16 q) noexcept {
    return pack(z, q);
}

inline constexpr U64 make_s(U16 q) noexcept {
    return pack(s, q);
}

inline constexpr U64 make_sdg(U16 q) noexcept {
    return pack(sdg, q);
}

inline constexpr U64 make_cnot(U16 control, U16 target) noexcept {
    return pack(cnot, control, target);
}

inline constexpr U64 make_cz(U16 a, U16 b) noexcept {
    if (a > b) std::swap(a, b);
    return pack(cz, a, b);
}

inline constexpr U64 make_toffoli(
    U16 control0, U16 control1, U16 target) noexcept {
    if (control0 > control1) std::swap(control0, control1);
    return pack(toffoli, control0, control1, target);
}

inline constexpr U64 make_ccz(U16 a, U16 b, U16 c) noexcept {
    if (a > b) std::swap(a, b);
    if (b > c) std::swap(b, c);
    if (a > b) std::swap(a, b);
    return pack(ccz, a, b, c);
}

inline constexpr unsigned arity(U16 op) noexcept {
    if (op == x || op == z || op == s || op == sdg) return 1;
    if (op == cnot || op == cz) return 2;
    if (op == toffoli || op == ccz) return 3;
    return 0;
}

inline void check_qubits(U64 gate, U32 qubits) {
    if (qubits == 0 || qubits > (U32{1} << 16)) {
        throw std::invalid_argument("a circuit must contain 1..65536 qubits");
    }
    const unsigned count = arity(opcode(gate));
    if (count == 0) throw std::invalid_argument("unknown packed gate opcode");
    for (unsigned i = 0; i < count; ++i) {
        if (qubit(gate, i) >= qubits) {
            throw std::invalid_argument("packed gate qubit is out of range");
        }
    }
}

inline bool commute(U64 first, U64 second) noexcept {
    const U16 first_op = opcode(first);
    const U16 second_op = opcode(second);
    if (first_op == cnot && second_op == cnot) {
        return qubit(first, 1) != qubit(second, 0) &&
               qubit(second, 1) != qubit(first, 0);
    }
    if (first_op == cnot && second_op == x) {
        return qubit(second, 0) != qubit(first, 0);
    }
    if (first_op == x && second_op == cnot) return commute(second, first);
    if (first_op == cnot && second_op == toffoli) {
        const U16 control = qubit(first, 0);
        const U16 target = qubit(first, 1);
        return target != qubit(second, 0) && target != qubit(second, 1) &&
               control != qubit(second, 2);
    }
    if (first_op == toffoli && second_op == cnot) return commute(second, first);
    if (first_op == x && second_op == x) return true;
    if (first_op == x && second_op == toffoli) {
        const U16 wire = qubit(first, 0);
        return wire == qubit(second, 2) ||
               (wire != qubit(second, 0) && wire != qubit(second, 1));
    }
    if (first_op == toffoli && second_op == x) return commute(second, first);
    return false;
}

inline void cancel_commuting_involutions(std::vector<U64>& circuit) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (std::size_t first = 0; first < circuit.size() && !changed; ++first) {
            if (opcode(circuit[first]) != x && opcode(circuit[first]) != cnot) {
                continue;
            }
            for (std::size_t second = first + 1; second < circuit.size(); ++second) {
                if (circuit[first] == circuit[second]) {
                    circuit.erase(circuit.begin() + static_cast<std::ptrdiff_t>(second));
                    circuit.erase(circuit.begin() + static_cast<std::ptrdiff_t>(first));
                    changed = true;
                    break;
                }
                if (!commute(circuit[first], circuit[second])) break;
            }
        }
    }
}

} // namespace cnot
