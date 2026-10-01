// Cubic-phase routing for a three-register accumulator map.
#pragma once

#include <cnot/gate.h>
#include <cnot/gl.h>
#include <cnot/qpr.h>
#include <core/bit_vec.h>
#include <unordered_dense.h>

#include <algorithm>
#include <array>
#include <memory>
#include <span>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace cnot::cpr_abc {

inline constexpr std::size_t target_width = 4;

struct Term { std::array<BitArr, 3> factor; };

struct Step {
    U16 term = 0;
    std::array<U16, 3> target{65535, 65535, 65535};
};

struct Trace {
    Step step;
    std::shared_ptr<const Trace> previous;
};

struct Problem {
    std::array<U16, 3> sizes{}, offsets{};
    bool shared_uv = false;
    std::vector<Term> terms;
    BitVec all;
    std::size_t beam_width = 0;
    U16 workers = 1;
};

struct State {
    std::array<std::vector<BitArr>, 3> rows;
    std::array<std::vector<BitArr>, 3> inverse;
    BitVec done;
    U32 route_cost = 0;
    std::shared_ptr<const Trace> trace;
};

struct LowerPhase {
    BitArr linear;
    BitVec quadratic;
    explicit LowerPhase(U16 n = 0)
        : quadratic(static_cast<std::size_t>(n) * (n - 1) / 2) {}
};

inline std::size_t edge_index(U16 n, U16 a, U16 b) noexcept {
    if (a > b) std::swap(a, b);
    return static_cast<std::size_t>(a) * (2 * n - a - 1) / 2 + b - a - 1;
}

inline U8 logical_axis(const Problem& p, U8 slot) noexcept {
    return p.shared_uv && slot == 1 ? 0 : slot;
}

inline U8 route_weight(const Problem& p, U8 axis) noexcept {
    return p.shared_uv && axis == 0 ? 2 : 1;
}

inline Problem make_problem(
    U16 na, U16 nb, U16 nc, std::span<const BitArr> u,
    std::span<const BitArr> v, std::span<const BitArr> w,
    std::size_t beam_width, U16 workers) {
    if (!na || !nb || !nc || U32{na} + nb + nc > 65535 ||
        na > BitArr::max_bits() || nb > BitArr::max_bits() || nc > BitArr::max_bits())
        throw std::invalid_argument("invalid ABC register sizes");
    if (u.empty() || u.size() != v.size() || u.size() != w.size() || u.size() > 65535)
        throw std::invalid_argument("U, V, and W must have the same nonzero rank");
    if (!beam_width || !workers)
        throw std::invalid_argument("beam width and worker count must be positive");
    Problem p;
    p.sizes = {na, nb, nc};
    p.offsets = {0, na, static_cast<U16>(na + nb)};
    p.shared_uv = na == nb && std::equal(u.begin(), u.end(), v.begin());
    p.all = BitVec(u.size());
    p.beam_width = beam_width;
    p.workers = workers;
    for (std::size_t q = 0; q < u.size(); ++q) {
        Term term{{u[q], v[q], w[q]}};
        for (U8 axis = 0; axis < 3; ++axis) {
            if (term.factor[axis].none() || !gl::in_range(term.factor[axis], p.sizes[axis]))
                throw std::invalid_argument("ABC factor is zero or out of range");
        }
        p.terms.push_back(term);
        p.all.set(q);
    }
    return p;
}

inline State initial_state(const Problem& p) {
    State state;
    for (U8 axis = 0; axis < 3; ++axis) {
        if (p.shared_uv && axis == 1) continue;
        state.rows[axis] = gl::identity<BitArr>(p.sizes[axis]);
        state.inverse[axis] = state.rows[axis];
    }
    state.done = BitVec(p.terms.size());
    return state;
}

inline U64 mix(U64 a, U64 b) noexcept {
    const U128 product = U128{a} * b;
    return U64(product) ^ U64(product >> 64);
}

inline U64 hash(U64 value) noexcept {
    return mix(value ^ 0x9e3779b97f4a7c15ULL, 0xe7037ed1a0b428dbULL);
}

inline U64 state_hash(const State& state) noexcept {
    std::size_t words = state.done.word_count();
    for (const auto& rows : state.rows) words += rows.size() * ((rows.size() + 63) / 64);
    U64 value = hash(words);
    for (const auto& rows : state.rows) {
        for (const BitArr& row : rows) {
            for (std::size_t i = 0; i < (rows.size() + 63) / 64; ++i)
                value = mix(value ^ row.words[i], 0x9e3779b97f4a7c15ULL);
        }
    }
    for (U64 word : state.done.words) value = mix(value ^ word, 0x9e3779b97f4a7c15ULL);
    return value;
}

inline U32 restoration_bound(const State& state, const Problem& p) {
    U32 result = 0;
    for (U8 axis = 0; axis < 3; ++axis) {
        for (U16 row = 0; row < state.rows[axis].size(); ++row)
            result += route_weight(p, axis) * (state.rows[axis][row] != gl::unit<BitArr>(row));
    }
    return result;
}

struct Candidate {
    std::size_t parent = 0;
    Step step;
    U32 route_cost = 0, restore = 0;
    U64 tie = 0;
};

inline bool candidate_less(const Candidate& a, const Candidate& b) {
    return std::tuple{U64{a.route_cost} + a.restore, a.tie} <
           std::tuple{U64{b.route_cost} + b.restore, b.tie};
}

inline void retain_targets(std::vector<Candidate>& plans) {
    const auto less = [](const Candidate& a, const Candidate& b) {
        return std::tuple{a.route_cost, a.tie} < std::tuple{b.route_cost, b.tie};
    };
    if (plans.size() > target_width) {
        std::nth_element(plans.begin(), plans.begin() + target_width, plans.end(), less);
        plans.resize(target_width);
    }
    std::sort(plans.begin(), plans.end(), less);
}

inline State materialize(const State& parent, const Candidate& plan, const Problem& p) {
    State child = parent;
    for (U8 axis = 0; axis < 3; ++axis) {
        if (p.shared_uv && axis == 1) continue;
        const U16 target = plan.step.target[axis];
        const BitArr& desired = p.terms[plan.step.term].factor[axis];
        BitArr coordinates = gl::coordinates(desired, parent.inverse[axis]);
        coordinates.flip(target);
        child.rows[axis][target] = desired;
        for (BitArr& row : child.inverse[axis]) {
            if (row.test(target)) row ^= coordinates;
        }
    }
    child.done.set(plan.step.term);
    child.route_cost = plan.route_cost;
    child.trace = std::make_shared<Trace>(Trace{plan.step, parent.trace});
    return child;
}

inline State find_route(const Problem& p) {
    std::vector<State> beam{initial_state(p)};
    for (std::size_t layer = 0; layer < p.terms.size(); ++layer) {
        std::vector<std::vector<Candidate>> per_parent(beam.size());
        gl::parallel_for(beam.size(), p.workers, [&](std::size_t parent_index) {
            const State& parent = beam[parent_index];
            const U32 restore = restoration_bound(parent, p);
            const U64 parent_hash = state_hash(parent);
            for (U16 q = 0; q < p.terms.size(); ++q) {
                if (parent.done.test(q)) continue;
                std::vector<Candidate> plans{{parent_index, Step{q}, parent.route_cost, restore, 0}};
                for (U8 axis = 0; axis < 3; ++axis) {
                    if (p.shared_uv && axis == 1) continue;
                    const BitArr& desired = p.terms[q].factor[axis];
                    const BitArr coordinates = gl::coordinates(desired, parent.inverse[axis]);
                    const U32 added = route_weight(p, axis) * static_cast<U32>(coordinates.count() - 1);
                    std::vector<Candidate> children;
                    for (const Candidate& prefix : plans) {
                        for (U16 target = 0; target < p.sizes[axis]; ++target) {
                            if (!coordinates.test(target)) continue;
                            Candidate child = prefix;
                            child.step.target[axis] = target;
                            child.route_cost += added;
                            const BitArr unit = gl::unit<BitArr>(target);
                            child.restore -= route_weight(p, axis) * (parent.rows[axis][target] != unit);
                            child.restore += route_weight(p, axis) * (desired != unit);
                            const auto& t = child.step.target;
                            U64 packed = U64{t[0]} | (U64{t[2]} << 16);
                            if (!p.shared_uv) packed |= U64{t[1]} << 32;
                            child.tie = mix(parent_hash ^ hash(q), hash(packed));
                            children.push_back(child);
                        }
                    }
                    retain_targets(children);
                    plans = std::move(children);
                }
                auto& output = per_parent[parent_index];
                output.insert(output.end(), plans.begin(), plans.end());
            }
        });
        std::vector<Candidate> candidates;
        std::size_t count = 0;
        for (const auto& batch : per_parent) count += batch.size();
        candidates.reserve(count);
        for (const auto& batch : per_parent) candidates.insert(candidates.end(), batch.begin(), batch.end());
        const auto worse = [](const Candidate& a, const Candidate& b) { return candidate_less(b, a); };
        std::make_heap(candidates.begin(), candidates.end(), worse);
        std::vector<State> next;
        ankerl::unordered_dense::set<U64> seen;
        next.reserve(std::min(p.beam_width, candidates.size()));
        seen.reserve(std::min(p.beam_width, candidates.size()));
        while (!candidates.empty() && next.size() < p.beam_width) {
            std::pop_heap(candidates.begin(), candidates.end(), worse);
            const Candidate plan = candidates.back();
            candidates.pop_back();
            State child = materialize(beam[plan.parent], plan, p);
            if (seen.insert(state_hash(child)).second) next.push_back(std::move(child));
        }
        if (next.empty()) throw std::runtime_error("ABC term beam is empty");
        beam = std::move(next);
    }
    std::vector<U64> cost(beam.size());
    gl::parallel_for(beam.size(), p.workers, [&](std::size_t i) {
        cost[i] = beam[i].route_cost;
        for (U8 axis = 0; axis < 3; ++axis) {
            if (!beam[i].rows[axis].empty())
                cost[i] += route_weight(p, axis) * gl::estimate<BitArr>(beam[i].rows[axis]);
        }
    });
    return std::move(beam[std::min_element(cost.begin(), cost.end()) - cost.begin()]);
}

inline std::vector<Step> unwind(std::shared_ptr<const Trace> trace) {
    std::vector<Step> result;
    for (; trace; trace = trace->previous) result.push_back(trace->step);
    std::reverse(result.begin(), result.end());
    return result;
}

inline std::vector<U64> phase_route(const Problem& p, const State& route) {
    auto rows = initial_state(p).rows;
    std::vector<U64> circuit;
    const auto emit_cnot = [&](U8 axis, U64 local) {
        gl::apply(rows[axis], local);
        circuit.push_back(make_cnot(p.offsets[axis] + qubit(local, 0), p.offsets[axis] + qubit(local, 1)));
        if (p.shared_uv && axis == 0)
            circuit.push_back(make_cnot(p.offsets[1] + qubit(local, 0), p.offsets[1] + qubit(local, 1)));
    };
    BitVec emitted(p.terms.size());
    for (const Step& step : unwind(route.trace)) {
        if (emitted.test(step.term)) throw std::runtime_error("ABC term emitted twice");
        std::array<U16, 3> wires{};
        for (U8 axis = 0; axis < 3; ++axis) {
            if (p.shared_uv && axis == 1) continue;
            const BitArr& desired = p.terms[step.term].factor[axis];
            const BitArr coordinates = gl::coordinates(desired, gl::inverse<BitArr>(rows[axis]));
            if (!coordinates.test(step.target[axis])) throw std::runtime_error("invalid ABC load target");
            for (U16 control = 0; control < rows[axis].size(); ++control) {
                if (control != step.target[axis] && coordinates.test(control))
                    emit_cnot(axis, make_cnot(control, step.target[axis]));
            }
            if (rows[axis][step.target[axis]] != desired) throw std::runtime_error("ABC load failed");
        }
        for (U8 slot = 0; slot < 3; ++slot)
            wires[slot] = p.offsets[slot] + step.target[logical_axis(p, slot)];
        circuit.push_back(make_ccz(wires[0], wires[1], wires[2]));
        emitted.set(step.term);
    }
    if (rows != route.rows || emitted != p.all ||
        circuit.size() - p.terms.size() != route.route_cost)
        throw std::runtime_error("ABC route replay mismatch");
    for (U8 axis = 0; axis < 3; ++axis) {
        if (rows[axis].empty()) continue;
        auto cleanup = gl::synt(p.sizes[axis], rows[axis], p.workers);
        gl::reverse(cleanup);
        for (U64 gate : cleanup) emit_cnot(axis, gate);
        if (!gl::is_identity<BitArr>(rows[axis])) throw std::runtime_error("ABC cleanup failed");
    }
    return circuit;
}

inline LowerPhase product(
    const BitArr& a, const BitArr& b, U16 qubits) {
    LowerPhase result(qubits);
    for (U16 i = 0; i < qubits; ++i) {
        if (a.test(i) && b.test(i)) result.linear.set(i);
        for (U16 j = i + 1; j < qubits; ++j) {
            if ((a.test(i) && b.test(j)) ^
                (a.test(j) && b.test(i))) {
                result.quadratic.set(edge_index(qubits, i, j));
            }
        }
    }
    return result;
}

struct FreeSite {
    std::size_t gate = 0;
    U8 slot = 0;
    LowerPhase phase;
};

struct ChargedSite {
    std::size_t boundary = 0;
    U16 a = 0;
    U16 b = 0;
    LowerPhase phase;
};

inline ChargedSite charged_site(
    U16 qubits, std::span<const BitArr> rows,
    std::size_t boundary, U16 a, U16 b) {
    if (a > b) std::swap(a, b);
    return ChargedSite{boundary, a, b, product(rows[a], rows[b], qubits)};
}

inline void append_unique(
    std::vector<ChargedSite>& sites,
    ankerl::unordered_dense::set<U64>& seen, ChargedSite site) {
    const U64 hash = ankerl::unordered_dense::hash<BitVec>{}(
        site.phase.quadratic);
    if (seen.insert(hash).second) sites.push_back(std::move(site));
}

inline BitVec coordinate_mask(U16 qubits, U16 c_offset) {
    BitVec result(static_cast<std::size_t>(qubits) * (qubits - 1) / 2);
    for (U16 input = 0; input < c_offset; ++input) {
        for (U16 accumulator = c_offset; accumulator < qubits; ++accumulator) {
            result.set(edge_index(qubits, input, accumulator));
        }
    }
    return result;
}

inline std::vector<ChargedSite> path_dictionary(
    U16 qubits, U16 c_offset, std::span<const U64> circuit,
    bool use_path_forms) {
    std::vector<ChargedSite> result;
    if (!use_path_forms) return result;

    ankerl::unordered_dense::set<U64> seen;
    const std::size_t quadratic_bits =
        static_cast<std::size_t>(qubits) * (qubits - 1) / 2;
    for (U16 input = 0; input < c_offset; ++input) {
        for (U16 accumulator = c_offset; accumulator < qubits; ++accumulator) {
            BitVec monomial(quadratic_bits);
            monomial.set(edge_index(qubits, input, accumulator));
            seen.insert(ankerl::unordered_dense::hash<BitVec>{}(monomial));
        }
    }

    std::vector<BitArr> rows = gl::identity<BitArr>(qubits);
    for (std::size_t index = 0; index < circuit.size(); ++index) {
        const U64 gate = circuit[index];
        if (opcode(gate) != cnot) continue;
        gl::apply(rows, gate);
        const U16 target = qubit(gate, 1);
        if (target < c_offset) {
            for (U16 accumulator = c_offset; accumulator < qubits; ++accumulator) {
                append_unique(result, seen, charged_site(
                    qubits, rows, index + 1, target, accumulator));
            }
        } else {
            for (U16 input = 0; input < c_offset; ++input) {
                append_unique(result, seen, charged_site(
                    qubits, rows, index + 1, input, target));
            }
        }
    }
    return result;
}

inline LowerPhase accumulator_phase(
    const Problem& problem, std::span<const BitArr> linear_a,
    std::span<const BitArr> linear_b, const BitArr& constant) {
    const U16 total = static_cast<U16>(
        problem.sizes[0] + problem.sizes[1] + problem.sizes[2]);
    if (total > BitArr::max_bits())
        throw std::invalid_argument("ABC affine phase exceeds BitArr capacity");
    if ((!linear_a.empty() && linear_a.size() != problem.sizes[2]) ||
        (!linear_b.empty() && linear_b.size() != problem.sizes[2]) ||
        !gl::in_range(constant, problem.sizes[2])) {
        throw std::invalid_argument("invalid affine accumulator dimensions");
    }
    LowerPhase result(total);
    for (U16 output = 0; output < problem.sizes[2]; ++output) {
        const U16 c = problem.offsets[2] + output;
        if (constant.test(output)) result.linear.set(c);
        if (!linear_a.empty()) {
            if (!gl::in_range(linear_a[output], problem.sizes[0])) {
                throw std::invalid_argument("linear A row is out of range");
            }
            for (U16 input = 0; input < problem.sizes[0]; ++input) {
                if (linear_a[output].test(input)) {
                    result.quadratic.set(edge_index(total, input, c));
                }
            }
        }
        if (!linear_b.empty()) {
            if (!gl::in_range(linear_b[output], problem.sizes[1])) {
                throw std::invalid_argument("linear B row is out of range");
            }
            for (U16 input = 0; input < problem.sizes[1]; ++input) {
                if (linear_b[output].test(input)) {
                    result.quadratic.set(edge_index(
                        total, problem.offsets[1] + input, c));
                }
            }
        }
    }
    return result;
}

inline std::vector<U64> to_nct(
    const Problem& problem, std::span<const U64> phase_circuit,
    std::span<const BitArr> linear_a, std::span<const BitArr> linear_b,
    const BitArr& constant, bool use_path_forms = true) {
    const U16 total = static_cast<U16>(
        problem.sizes[0] + problem.sizes[1] + problem.sizes[2]);
    if ((!linear_a.empty() && linear_a.size() != problem.sizes[2]) ||
        (!linear_b.empty() && linear_b.size() != problem.sizes[2]) ||
        !gl::in_range(constant, problem.sizes[2]))
        throw std::invalid_argument("invalid affine accumulator dimensions");
    bool has_quadratic = false;
    for (U8 axis = 0; axis < 2; ++axis) {
        for (const BitArr& row : axis == 0 ? linear_a : linear_b) {
            if (!gl::in_range(row, problem.sizes[axis]))
                throw std::invalid_argument("affine input row is out of range");
            has_quadratic |= row.any();
        }
    }
    if (!has_quadratic) {
        std::vector<U64> result;
        result.reserve(phase_circuit.size() + constant.count());
        for (const U64 gate : phase_circuit) {
            if (opcode(gate) == cnot) {
                const U16 control = qubit(gate, 0);
                const U16 target = qubit(gate, 1);
                result.push_back(control >= problem.offsets[2]
                    ? make_cnot(target, control)
                    : make_cnot(control, target));
            } else if (opcode(gate) == ccz) {
                result.push_back(make_toffoli(
                    qubit(gate, 0), qubit(gate, 1), qubit(gate, 2)));
            }
        }
        for (U16 output = 0; output < problem.sizes[2]; ++output) {
            if (constant.test(output)) {
                result.push_back(make_x(problem.offsets[2] + output));
            }
        }
        return result;
    }
    if (total > BitArr::max_bits())
        throw std::invalid_argument("ABC affine phase exceeds BitArr capacity");
    LowerPhase correction = accumulator_phase(
        problem, linear_a, linear_b, constant);
    std::vector<BitArr> rows = gl::identity<BitArr>(total);
    std::vector<FreeSite> free;
    for (std::size_t index = 0; index < phase_circuit.size(); ++index) {
        const U64 gate = phase_circuit[index];
        if (opcode(gate) == cnot) {
            gl::apply(rows, gate);
        } else if (opcode(gate) == ccz) {
            const std::array<U16, 3> wire{
                qubit(gate, 0), qubit(gate, 1), qubit(gate, 2)};
            for (U8 slot = 0; slot < 2; ++slot) {
                free.push_back(FreeSite{
                    index, slot, product(
                        rows[wire[(slot + 1) % 3]],
                        rows[wire[(slot + 2) % 3]], total)});
            }
        }
    }
    if (!gl::is_identity<BitArr>(rows)) {
        throw std::runtime_error("ABC phase route is not closed");
    }

    const std::vector<ChargedSite> path = path_dictionary(
        total, problem.offsets[2], phase_circuit, use_path_forms);
    std::vector<BitVec> free_forms;
    for (const FreeSite& site : free) free_forms.push_back(site.phase.quadratic);
    std::vector<BitVec> path_forms;
    for (const ChargedSite& site : path) {
        path_forms.push_back(site.phase.quadratic);
    }
    const qpr::Solution solution = qpr::solve(
        correction.quadratic, free_forms, path_forms,
        coordinate_mask(total, problem.offsets[2]));

    std::vector<std::vector<std::size_t>> at(phase_circuit.size() + 1);
    for (std::size_t i = 0; i < path.size(); ++i) {
        if (solution.charged.test(i)) at[path[i].boundary].push_back(i);
    }
    std::vector<std::array<int, 2>> x_source(
        phase_circuit.size(), {-1, -1});
    for (std::size_t i = 0; i < free.size(); ++i) {
        x_source[free[i].gate][free[i].slot] = static_cast<int>(i);
        if (solution.free.test(i)) correction.linear ^= free[i].phase.linear;
    }
    for (std::size_t i = 0; i < path.size(); ++i) {
        if (solution.charged.test(i)) correction.linear ^= path[i].phase.linear;
    }

    std::vector<U64> result;
    rows = gl::identity<BitArr>(total);
    std::vector<U8> offsets(total);
    const auto emit_x = [&](U16 wire) {
        result.push_back(make_x(wire));
        offsets[wire] ^= 1;
    };
    const auto emit_cnot = [&](U16 control, U16 target) {
        result.push_back(make_cnot(control, target));
        offsets[target] ^= offsets[control];
    };
    for (std::size_t index = 0; index <= phase_circuit.size(); ++index) {
        for (const std::size_t selected : at[index]) {
            const U16 a = path[selected].a;
            const U16 b = path[selected].b;
            const bool a_is_c = a >= problem.offsets[2];
            const bool b_is_c = b >= problem.offsets[2];
            if (a_is_c == b_is_c) {
                throw std::runtime_error(
                    "ABC charged phase does not map to an NCT CNOT");
            }
            if (a_is_c) emit_cnot(b, a);
            else emit_cnot(a, b);
        }
        if (index == phase_circuit.size()) {
            for (U16 input = 0; input < problem.offsets[2]; ++input) {
                for (U16 accumulator = problem.offsets[2];
                     accumulator < total; ++accumulator) {
                    if (solution.coordinate.test(
                            edge_index(total, input, accumulator))) {
                        emit_cnot(input, accumulator);
                    }
                }
            }
            break;
        }
        const U64 gate = phase_circuit[index];
        if (opcode(gate) == cnot) {
            const U16 control = qubit(gate, 0);
            const U16 target = qubit(gate, 1);
            const bool in_c = control >= problem.offsets[2];
            if (in_c) emit_cnot(target, control);
            else emit_cnot(control, target);
            gl::apply(rows, gate);
        } else if (opcode(gate) == ccz) {
            const std::array<U16, 3> wire{
                qubit(gate, 0), qubit(gate, 1), qubit(gate, 2)};
            U8 mask = 0;
            for (U8 slot = 0; slot < 2; ++slot) {
                const int source = x_source[index][slot];
                if (source >= 0 && solution.free.test(static_cast<std::size_t>(source))) {
                    mask |= U8{1} << slot;
                }
                if (offsets[wire[slot]] != ((mask >> slot) & 1)) {
                    emit_x(wire[slot]);
                }
            }
            result.push_back(make_toffoli(wire[0], wire[1], wire[2]));
            if (mask == 3) correction.linear ^= rows[wire[2]];
        }
    }
    for (U16 wire = 0; wire < total; ++wire) {
        const bool desired = correction.linear.test(wire);
        if (desired && wire < problem.offsets[2]) {
            throw std::runtime_error("ABC correction leaves an input phase");
        }
        if (offsets[wire] != desired) emit_x(wire);
    }
    cancel_commuting_involutions(result);
    return result;
}

inline std::vector<U64> synt(
    U16 na, U16 nb, U16 nc,
    std::span<const BitArr> u, std::span<const BitArr> v,
    std::span<const BitArr> w, std::size_t beam_width,
    U16 workers = 1, bool use_path_forms = true) {
    const Problem problem = make_problem(
        na, nb, nc, u, v, w, beam_width, workers);
    const State route = find_route(problem);
    const std::vector<U64> phase = phase_route(problem, route);
    return to_nct(problem, phase, {}, {}, BitArr{}, use_path_forms);
}

inline std::vector<U64> synt(
    U16 na, U16 nb, U16 nc,
    std::span<const BitArr> u, std::span<const BitArr> v,
    std::span<const BitArr> w,
    std::span<const BitArr> linear_a,
    std::span<const BitArr> linear_b, const BitArr& constant,
    std::size_t beam_width, U16 workers = 1,
    bool use_path_forms = true) {
    const Problem problem = make_problem(
        na, nb, nc, u, v, w, beam_width, workers);
    const State route = find_route(problem);
    const std::vector<U64> phase = phase_route(problem, route);
    return to_nct(
        problem, phase, linear_a, linear_b, constant, use_path_forms);
}

} // namespace cnot::cpr_abc
