// gl.h
#pragma once

#include <cnot/gate.h>
#include <core/bit_arr.h>

#include <algorithm>
#include <atomic>
#include <bit>
#include <cstddef>
#include <limits>
#include <span>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// Greedy-PMH CNOT synthesis for a row-major matrix M in GL(n,2).
namespace cnot::gl {

template <class Row>
inline bool bit(const Row& row, U16 index) noexcept {
    if constexpr (std::is_same_v<Row, U64>) {
        return ((row >> index) & 1) != 0;
    } else {
        return row.test(index);
    }
}

template <class Row>
inline Row unit(U16 index) noexcept {
    Row row{};
    if constexpr (std::is_same_v<Row, U64>) {
        row = U64{1} << index;
    } else {
        row.set(index);
    }
    return row;
}

template <class Row>
inline bool in_range(const Row& row, U16 n) noexcept {
    if constexpr (std::is_same_v<Row, U64>) {
        const U64 mask = n == 64 ? ~U64{0} : (U64{1} << n) - 1;
        return (row & ~mask) == 0;
    } else {
        for (std::size_t i = n; i < Row::max_bits(); ++i) {
            if (row.test(i)) return false;
        }
        return true;
    }
}

template <class Row>
inline std::vector<Row> identity(U16 n) {
    std::vector<Row> rows(n);
    for (U16 i = 0; i < n; ++i) rows[i] = unit<Row>(i);
    return rows;
}

template <class Row>
inline void apply(std::vector<Row>& rows, U64 gate) {
    if (opcode(gate) != cnot) {
        throw std::invalid_argument("GL row operation must be a CNOT");
    }
    rows[qubit(gate, 1)] ^= rows[qubit(gate, 0)];
}

template <class Row>
inline bool is_identity(std::span<const Row> rows) {
    for (U16 i = 0; i < rows.size(); ++i) {
        if (rows[i] != unit<Row>(i)) return false;
    }
    return true;
}

template <class Row>
inline bool is_identity(const std::vector<Row>& rows) {
    return is_identity<Row>(std::span<const Row>(rows));
}

template <class Row>
inline std::vector<Row> transpose(std::span<const Row> rows) {
    const U16 n = static_cast<U16>(rows.size());
    std::vector<Row> result(n);
    for (U16 i = 0; i < n; ++i) {
        for (U16 j = 0; j < n; ++j) {
            if (bit(rows[i], j)) {
                if constexpr (std::is_same_v<Row, U64>) {
                    result[j] |= U64{1} << i;
                } else {
                    result[j].set(i);
                }
            }
        }
    }
    return result;
}

template <class Row>
inline std::vector<Row> inverse(std::span<const Row> matrix) {
    const U16 n = static_cast<U16>(matrix.size());
    std::vector<Row> rows(matrix.begin(), matrix.end());
    std::vector<Row> result = identity<Row>(n);
    for (U16 column = 0; column < n; ++column) {
        if (!bit(rows[column], column)) {
            U16 pivot = column + 1;
            while (pivot < n && !bit(rows[pivot], column)) ++pivot;
            if (pivot == n) throw std::invalid_argument("matrix is singular");
            const U64 gate = make_cnot(pivot, column);
            apply(rows, gate);
            apply(result, gate);
        }
        for (U16 row = 0; row < n; ++row) {
            if (row != column && bit(rows[row], column)) {
                const U64 gate = make_cnot(column, row);
                apply(rows, gate);
                apply(result, gate);
            }
        }
    }
    return result;
}

template <class Row>
inline std::vector<Row> inverse(const std::vector<Row>& matrix) {
    return inverse<Row>(std::span<const Row>(matrix));
}

template <class Row>
inline Row coordinates(
    const Row& form, std::span<const Row> inverse_rows) {
    Row result{};
    for (U16 i = 0; i < inverse_rows.size(); ++i) {
        if (bit(form, i)) result ^= inverse_rows[i];
    }
    return result;
}

template <class Row>
inline Row coordinates(
    const Row& form, const std::vector<Row>& inverse_rows) {
    return coordinates<Row>(form, std::span<const Row>(inverse_rows));
}

inline auto cnot_key(U64 gate) noexcept {
    return std::tuple{qubit(gate, 0), qubit(gate, 1)};
}

inline bool circuit_less(
    std::span<const U64> left, std::span<const U64> right) {
    return std::lexicographical_compare(
        left.begin(), left.end(), right.begin(), right.end(),
        [](U64 a, U64 b) { return cnot_key(a) < cnot_key(b); });
}

inline bool better(
    std::span<const U64> left, std::span<const U64> right) {
    return left.size() < right.size() ||
           (left.size() == right.size() && circuit_less(left, right));
}

template <class Row>
inline U16 pattern(
    const Row& row, U16 start, U16 length) noexcept {
    U16 result = 0;
    for (U16 i = 0; i < length; ++i) {
        if (bit(row, start + i)) result |= U16{1} << i;
    }
    return result;
}

// Reduce the lower triangular part in PMH block order.  The returned CNOTs
// are row operations applied to `rows` in the order in which they occur.
template <class Row>
inline std::vector<U64> lower_pmh(
    std::vector<Row>& rows, U16 block) {
    const U16 n = static_cast<U16>(rows.size());
    std::vector<U64> result;
    for (U16 start = 0; start < n; start += block) {
        const U16 end = std::min<U16>(n, start + block);
        const U16 length = end - start;
        std::vector<int> first(std::size_t{1} << length, -1);
        for (U16 row = start; row < n; ++row) {
            const U16 value = pattern(rows[row], start, length);
            if (value == 0) continue;
            if (first[value] < 0) {
                first[value] = row;
            } else {
                const U64 gate = make_cnot(
                    static_cast<U16>(first[value]), row);
                apply(rows, gate);
                result.push_back(gate);
            }
        }
        for (U16 column = start; column < end; ++column) {
            if (!bit(rows[column], column)) {
                U16 pivot = column + 1;
                while (pivot < n && !bit(rows[pivot], column)) ++pivot;
                if (pivot == n) throw std::invalid_argument("matrix is singular");
                const U64 gate = make_cnot(pivot, column);
                apply(rows, gate);
                result.push_back(gate);
            }
            for (U16 row = column + 1; row < n; ++row) {
                if (bit(rows[row], column)) {
                    const U64 gate = make_cnot(column, row);
                    apply(rows, gate);
                    result.push_back(gate);
                }
            }
        }
    }
    return result;
}

template <class Row>
inline U32 lower_pmh_cost(std::vector<Row>& rows, U16 block) {
    const U16 n = static_cast<U16>(rows.size());
    U32 result = 0;
    for (U16 start = 0; start < n; start += block) {
        const U16 end = std::min<U16>(n, start + block);
        const U16 length = end - start;
        std::vector<int> first(std::size_t{1} << length, -1);
        for (U16 row = start; row < n; ++row) {
            const U16 value = pattern(rows[row], start, length);
            if (value == 0) continue;
            if (first[value] < 0) {
                first[value] = row;
            } else {
                apply(rows, make_cnot(static_cast<U16>(first[value]), row));
                ++result;
            }
        }
        for (U16 column = start; column < end; ++column) {
            if (!bit(rows[column], column)) {
                U16 pivot = column + 1;
                while (pivot < n && !bit(rows[pivot], column)) ++pivot;
                if (pivot == n) throw std::invalid_argument("matrix is singular");
                apply(rows, make_cnot(pivot, column));
                ++result;
            }
            for (U16 row = column + 1; row < n; ++row) {
                if (bit(rows[row], column)) {
                    apply(rows, make_cnot(column, row));
                    ++result;
                }
            }
        }
    }
    return result;
}

template <class Row>
inline std::vector<U64> pmh(
    std::span<const Row> matrix, U16 block) {
    std::vector<Row> rows(matrix.begin(), matrix.end());
    const std::vector<U64> lower = lower_pmh(rows, block);
    rows = transpose<Row>(rows);
    const std::vector<U64> upper = lower_pmh(rows, block);
    if (!is_identity<Row>(rows)) {
        throw std::runtime_error("PMH synthesis failed");
    }

    std::vector<U64> result;
    result.reserve(lower.size() + upper.size());
    for (const U64 gate : upper) {
        result.push_back(make_cnot(qubit(gate, 1), qubit(gate, 0)));
    }
    for (auto it = lower.rbegin(); it != lower.rend(); ++it) {
        result.push_back(*it);
    }
    return result;
}

template <class Row>
inline std::vector<U64> pmh(std::span<const Row> matrix) {
    const U16 n = static_cast<U16>(matrix.size());
    const U16 block = std::min<U16>(
        8, std::max<U16>(1, static_cast<U16>(std::bit_width(n)) - 1));
    return pmh<Row>(matrix, block);
}

template <class Row>
inline U32 fast_estimate(std::span<const Row> matrix) {
    const U16 n = static_cast<U16>(matrix.size());
    const U16 block = std::min<U16>(
        8, std::max<U16>(1, static_cast<U16>(std::bit_width(n)) - 1));
    std::vector<Row> rows(matrix.begin(), matrix.end());
    U32 result = lower_pmh_cost(rows, block);
    rows = transpose<Row>(rows);
    result += lower_pmh_cost(rows, block);
    if (!is_identity<Row>(rows)) {
        throw std::runtime_error("PMH estimate failed");
    }
    return result;
}

template <class Row>
inline U32 estimate(std::span<const Row> matrix) {
    return fast_estimate<Row>(matrix);
}

inline void reverse(std::vector<U64>& circuit) {
    std::reverse(circuit.begin(), circuit.end());
}

template <class Function>
inline void parallel_for(
    std::size_t count, U16 workers, const Function& function) {
    if (workers <= 1 || count < 2) {
        for (std::size_t i = 0; i < count; ++i) function(i);
        return;
    }
    std::atomic_size_t next{0};
    const U16 active = static_cast<U16>(
        std::min<std::size_t>(workers, count));
    std::vector<std::thread> threads;
    threads.reserve(active);
    for (U16 worker = 0; worker < active; ++worker) {
        threads.emplace_back([&] {
            while (true) {
                const std::size_t i = next.fetch_add(1);
                if (i >= count) break;
                function(i);
            }
        });
    }
    for (std::thread& thread : threads) thread.join();
}

// Prepend a CNOT only when it improves the PMH suffix by at least two gates.
template <class Row>
inline std::vector<U64> reduce(
    std::span<const Row> matrix, U16 workers) {
    const U16 n = static_cast<U16>(matrix.size());
    std::vector<Row> current(matrix.begin(), matrix.end());
    std::vector<U64> prefix;
    std::vector<U64> suffix = pmh<Row>(current);
    reverse(suffix);
    std::vector<U64> best = suffix;

    while (true) {
        const std::size_t choices = static_cast<std::size_t>(n) * (n - 1);
        if (choices == 0) break;
        std::vector<U32> costs(choices);
        parallel_for(choices, workers, [&](std::size_t index) {
            const U16 control = static_cast<U16>(index / (n - 1));
            U16 target = static_cast<U16>(index % (n - 1));
            if (target >= control) ++target;
            std::vector<Row> candidate = current;
            apply(candidate, make_cnot(control, target));
            costs[index] = estimate<Row>(candidate);
        });

        U32 selected_cost = std::numeric_limits<U32>::max();
        U64 selected = 0;
        for (std::size_t index = 0; index < choices; ++index) {
            const U16 control = static_cast<U16>(index / (n - 1));
            U16 target = static_cast<U16>(index % (n - 1));
            if (target >= control) ++target;
            const U64 gate = make_cnot(control, target);
            if (std::tuple{costs[index], cnot_key(gate)} <
                std::tuple{selected_cost, cnot_key(selected)}) {
                selected_cost = costs[index];
                selected = gate;
            }
        }
        if (selected_cost + 1 >= suffix.size()) break;

        apply(current, selected);
        prefix.push_back(selected);
        suffix = pmh<Row>(current);
        reverse(suffix);
        std::vector<U64> candidate = prefix;
        candidate.insert(candidate.end(), suffix.begin(), suffix.end());
        if (better(candidate, best)) best = std::move(candidate);
    }
    return best;
}

template <class Row>
inline bool realizes(
    U16 n, std::span<const Row> matrix,
    std::span<const U64> circuit) {
    std::vector<Row> rows = identity<Row>(n);
    for (const U64 gate : circuit) apply(rows, gate);
    return std::equal(rows.begin(), rows.end(), matrix.begin(), matrix.end());
}

template <class Row>
inline void validate(U16 n, std::span<const Row> matrix) {
    if (n == 0 || matrix.size() != n) {
        throw std::invalid_argument("GL synthesis requires an n by n matrix");
    }
    if constexpr (std::is_same_v<Row, U64>) {
        if (n > 64) throw std::invalid_argument("a U64 matrix supports at most 64 rows");
    } else {
        if (n > Row::max_bits()) {
            throw std::invalid_argument("matrix exceeds the row capacity");
        }
    }
    for (const Row& row : matrix) {
        if (!in_range(row, n)) {
            throw std::invalid_argument("GL row is out of range");
        }
    }
}

template <class Row>
inline std::vector<U64> synt_rows(
    U16 n, std::span<const Row> matrix, U16 workers) {
    validate(n, matrix);
    if (workers == 0) throw std::invalid_argument("GL workers must be positive");
    std::vector<U64> circuit = reduce<Row>(matrix, workers);
    reverse(circuit);
    if (!realizes<Row>(n, matrix, circuit)) {
        throw std::runtime_error("internal greedy-PMH verification failed");
    }
    return circuit;
}

inline std::vector<U64> synt(std::span<const U64> matrix) {
    return synt_rows<U64>(
        static_cast<U16>(matrix.size()), matrix, 1);
}

inline std::vector<U64> synt(
    U16 n, std::span<const BitArr> matrix, U16 workers = 1) {
    return synt_rows<BitArr>(n, matrix, workers);
}

} // namespace cnot::gl
