// Greedy quadratic-phase routing with free, coordinate, and path forms.
#pragma once

#include <core/bit_vec.h>

#include <bit>
#include <cstddef>
#include <optional>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cnot::qpr {

struct Solution {
    BitVec free;
    BitVec charged;
    BitVec coordinate;
};

struct BasisRow {
    BitVec value;
    BitVec witness;
};

inline void check_width(const BitVec& value, std::size_t words) {
    if (value.word_count() != words) {
        throw std::invalid_argument("quadratic forms must have equal widths");
    }
}

inline std::size_t last_one(const BitVec& value) noexcept {
    for (std::size_t i = value.word_count(); i > 0; --i) {
        const U64 word = value.words[i - 1];
        if (word != 0) {
            return (i - 1) * BitVec::kWordBits + BitVec::kWordBits - 1 -
                static_cast<std::size_t>(std::countl_zero(word));
        }
    }
    return value.capacity_bits();
}

inline bool is_subset(const BitVec& value, const BitVec& mask) noexcept {
    for (std::size_t i = 0; i < value.word_count(); ++i) {
        if ((value.words[i] & ~mask.words[i]) != 0) return false;
    }
    return true;
}

inline void verify(
    const BitVec& q, std::span<const BitVec> free,
    std::span<const BitVec> charged, const Solution& solution) {
    check_width(solution.coordinate, q.word_count());
    BitVec check = solution.coordinate;
    for (std::size_t i = 0; i < free.size(); ++i) {
        if (solution.free.test(i)) check ^= free[i];
    }
    for (std::size_t i = 0; i < charged.size(); ++i) {
        if (solution.charged.test(i)) check ^= charged[i];
    }
    if (check != q) {
        throw std::runtime_error("quadratic decoder returned an invalid witness");
    }
}

// Coordinate monomials are represented implicitly by coordinate_mask.  The
// charged dictionary contains only additional forms available along the path.
inline Solution solve(
    const BitVec& q, std::span<const BitVec> free,
    std::span<const BitVec> charged, const BitVec& coordinate_mask) {
    const std::size_t words = q.word_count();
    check_width(coordinate_mask, words);
    const auto check = [&](const BitVec& value) {
        check_width(value, words);
        if (!is_subset(value, coordinate_mask)) {
            throw std::invalid_argument("quadratic form lies outside coordinate dictionary");
        }
    };
    check(q);
    for (const BitVec& value : free) check(value);
    for (const BitVec& value : charged) check(value);

    const std::size_t dimension = q.capacity_bits();
    std::vector<std::optional<BasisRow>> basis(dimension);
    for (std::size_t i = 0; i < free.size(); ++i) {
        BasisRow row{free[i], BitVec(free.size())};
        row.witness.set(i);
        while (row.value.any()) {
            const std::size_t pivot = last_one(row.value);
            if (!basis[pivot]) {
                basis[pivot] = std::move(row);
                break;
            }
            row.value ^= basis[pivot]->value;
            row.witness ^= basis[pivot]->witness;
        }
    }
    Solution result{BitVec(free.size()), BitVec(charged.size()), q};
    for (std::size_t pivot = dimension; pivot-- > 0;) {
        if (basis[pivot] && result.coordinate.test(pivot)) {
            result.coordinate ^= basis[pivot]->value;
            result.free ^= basis[pivot]->witness;
        }
    }
    // Use the lighter feasible seed; ties retain the Gaussian representative.
    if (result.coordinate.count() > q.count()) {
        result.coordinate = q;
        result.free.reset();
    }

    // Preserve Q = F*free + D*charged + coordinate using physical insertions.
    bool changed = true;
    while (changed) {
        changed = false;
        const auto descend = [&](std::span<const BitVec> forms, BitVec& selected, bool paid) {
            for (std::size_t i = 0; i < forms.size(); ++i) {
                const auto delta = static_cast<std::ptrdiff_t>(forms[i].count()) -
                    2 * static_cast<std::ptrdiff_t>(and_count(result.coordinate, forms[i])) +
                    (paid ? (selected.test(i) ? -1 : 1) : 0);
                if (delta >= 0) continue;
                result.coordinate ^= forms[i];
                selected.flip(i);
                changed = true;
            }
        };
        descend(free, result.free, false);
        descend(charged, result.charged, true);
    }

    verify(q, free, charged, result);
    return result;
}

} // namespace cnot::qpr
