// Cubic-phase routing when all UVW factors share one qubit register.
#pragma once

#include <cnot/gate.h>
#include <cnot/gl.h>
#include <cnot/qpr.h>
#include <core/bit_vec.h>
#include <unordered_dense.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace cnot::cpr_xxx {

using Variant = std::array<U8, 3>;
inline constexpr std::size_t target_width = 4;

struct Term {
    std::array<U32, 7> forms{};
};

struct Load {
    U32 form = 0;
    U16 target = 0;
};

struct Trace {
    U16 term = 0;
    Variant variant{};
    std::array<Load, 3> loads{};
    U8 load_count = 0;
    std::shared_ptr<const Trace> previous;
};

struct Problem {
    U16 qubits = 0;
    std::vector<Term> terms;
    std::vector<BitArr> forms;
    std::size_t beam_width = 0;
    U16 workers = 1;
};

struct State {
    std::vector<BitArr> rows;
    std::vector<BitArr> inverse;
    BitVec done;
    U32 route_cost = 0;
    U32 changed_rows = 0;
    U64 hash = 0;
    U64 order = 0;
    std::shared_ptr<const Trace> trace;
};

struct LowerPhase {
    BitArr linear;
    BitVec quadratic;

    explicit LowerPhase(U16 qubits = 0)
        : quadratic(static_cast<std::size_t>(qubits) * (qubits - 1) / 2) {}

    LowerPhase& operator^=(const LowerPhase& other) {
        linear ^= other.linear;
        quadratic ^= other.quadratic;
        return *this;
    }
};

inline std::size_t edge_index(U16 n, U16 a, U16 b) noexcept {
    if (a > b) std::swap(a, b);
    return static_cast<std::size_t>(a) * (2 * n - a - 1) / 2 + b - a - 1;
}

inline const std::array<Variant, 28>& variants() {
    static constexpr auto value = [] {
        std::array<Variant, 28> result{};
        std::size_t index = 0;
        for (U8 a = 1; a <= 7; ++a) {
            for (U8 b = a + 1; b <= 7; ++b) {
                for (U8 c = b + 1; c <= 7; ++c) {
                    if ((a ^ b) != c) result[index++] = {a, b, c};
                }
            }
        }
        return result;
    }();
    return value;
}

inline BitArr xor_masked(
    const BitArr& u, const BitArr& v, const BitArr& w, U8 mask) {
    BitArr result;
    if (mask & 1) result ^= u;
    if (mask & 2) result ^= v;
    if (mask & 4) result ^= w;
    return result;
}

inline bool independent(
    const BitArr& u, const BitArr& v, const BitArr& w) noexcept {
    return u != v && u != w && v != w && (u ^ v) != w;
}


inline Problem make_problem(
    U16 qubits, std::span<const BitArr> u,
    std::span<const BitArr> v, std::span<const BitArr> w,
    std::size_t beam_width, U16 workers) {
    if (qubits == 0 || qubits > BitArr::max_bits()) {
        throw std::invalid_argument("cpr_xxx exceeds BitArr capacity");
    }
    if (u.empty() || u.size() != v.size() || u.size() != w.size() ||
        u.size() > 65535) {
        throw std::invalid_argument("U, V, and W must have the same nonzero rank");
    }
    if (beam_width == 0 || workers == 0) {
        throw std::invalid_argument("beam width and worker count must be positive");
    }
    Problem problem{qubits, std::vector<Term>(u.size()), {}, beam_width, workers};
    ankerl::unordered_dense::map<BitArr, U32> ids;
    for (std::size_t term = 0; term < u.size(); ++term) {
        if (u[term].none() || v[term].none() || w[term].none() ||
            !gl::in_range(u[term], qubits) ||
            !gl::in_range(v[term], qubits) ||
            !gl::in_range(w[term], qubits) ||
            !independent(u[term], v[term], w[term])) {
            throw std::invalid_argument(
                "every UVW term must contain three independent forms");
        }
        for (U8 mask = 1; mask <= 7; ++mask) {
            const BitArr value = xor_masked(u[term], v[term], w[term], mask);
            const auto [entry, inserted] = ids.emplace(
                value, static_cast<U32>(problem.forms.size()));
            if (inserted) problem.forms.push_back(value);
            problem.terms[term].forms[mask - 1] = entry->second;
        }
    }
    return problem;
}

inline int row_index(std::span<const BitArr> rows, const BitArr& value) {
    const auto found = std::find(rows.begin(), rows.end(), value);
    return found == rows.end() ? -1 : static_cast<int>(found - rows.begin());
}

inline U64 mix_hash(U64 a, U64 b) noexcept {
    const U128 product = U128{a} * b;
    return U64(product) ^ U64(product >> 64);
}

inline U64 row_hash(U16 index, const BitArr& row, std::size_t words) noexcept {
    U64 hash = mix_hash(U64{index} ^ 0x243f6a8885a308d3ULL, 0xe7037ed1a0b428dbULL);
    for (std::size_t j = 0; j < words; ++j) {
        hash = mix_hash(hash ^ row.words[j], 0x9e3779b97f4a7c15ULL);
    }
    return hash;
}

inline U64 term_hash(U16 term) noexcept {
    return mix_hash(U64{term} ^ 0xa4093822299f31d0ULL, 0xe7037ed1a0b428dbULL);
}

inline U64 state_hash(const State& state) noexcept {
    U64 hash = 0x13198a2e03707344ULL;
    const std::size_t words = (state.rows.size() + 63) / 64;
    for (U16 i = 0; i < state.rows.size(); ++i) {
        hash ^= row_hash(i, state.rows[i], words);
    }
    for (std::size_t i = 0; i < state.done.words.size(); ++i) {
        U64 terms = state.done.words[i];
        while (terms) {
            hash ^= term_hash(static_cast<U16>(64 * i + std::countr_zero(terms)));
            terms &= terms - 1;
        }
    }
    return hash;
}

inline U64 load_hash(const State& state, U16 target, const BitArr& value) noexcept {
    const std::size_t words = (state.rows.size() + 63) / 64;
    return state.hash ^ row_hash(target, state.rows[target], words) ^
        row_hash(target, value, words);
}

inline State initial_state(const Problem& problem) {
    State state;
    state.rows = gl::identity<BitArr>(problem.qubits);
    state.inverse = state.rows;
    state.done = BitVec(problem.terms.size());
    state.hash = state_hash(state);
    return state;
}

inline auto state_key(const State& state) {
    return std::tuple{
        U64{state.route_cost} + state.changed_rows,
        state.route_cost, state.hash, state.order};
}

// Each changed row requires at least one CNOT before restoring the identity.
inline bool state_less(const State& a, const State& b) {
    return state_key(a) < state_key(b);
}

struct Beam {
    std::size_t width;
    std::vector<State> states;
    ankerl::unordered_dense::set<U64> hashes;

    explicit Beam(std::size_t width) : width(width) {
        states.reserve(width);
        hashes.reserve(width);
    }

    void add(State state) {
        if (states.size() == width && !state_less(state, states.back())) return;
        if (!hashes.insert(state.hash).second) {
            const auto duplicate = std::find_if(
                states.begin(), states.end(),
                [&](const State& prior) { return prior.hash == state.hash; });
            if (!state_less(state, *duplicate)) return;
            states.erase(duplicate);
        }
        const auto position = std::lower_bound(
            states.begin(), states.end(), state, state_less);
        states.insert(position, std::move(state));
        if (states.size() > width) {
            hashes.erase(states.back().hash);
            states.pop_back();
        }
    }
};

struct Plan {
    State state;
    std::array<Load, 3> loads{};
    U8 load_count = 0;
};

inline std::vector<State> complete_term(
    const State& parent, const Problem& problem, U16 term) {
    std::array<U32, 8> weights{};
    for (U8 mask = 1; mask <= 7; ++mask) {
        const U32 form = problem.terms[term].forms[mask - 1];
        weights[mask] = static_cast<U32>(
            gl::coordinates(problem.forms[form], parent.inverse).count());
    }
    const auto variant_key = [&](const Variant& v) {
        return std::tuple{weights[v[0]] + weights[v[1]] + weights[v[2]], v};
    };
    const Variant variant = *std::min_element(
        variants().begin(), variants().end(),
        [&](const Variant& a, const Variant& b) {
            return variant_key(a) < variant_key(b);
        });
    std::array<std::pair<U32, U32>, 3> ordered_forms{};
    for (U8 slot = 0; slot < 3; ++slot) {
        ordered_forms[slot] = {
            weights[variant[slot]], problem.terms[term].forms[variant[slot] - 1]};
    }
    std::sort(ordered_forms.begin(), ordered_forms.end());
    std::array<U32, 3> forms{};
    for (U8 slot = 0; slot < 3; ++slot) forms[slot] = ordered_forms[slot].second;

    std::vector<Plan> plans{{parent}};
    for (U32 form : forms) {
        struct Target {
            std::size_t parent = 0;
            U16 row = no_qubit;
            U32 cost = 0;
            U64 hash = 0;
        };
        std::vector<Target> targets;
        const BitArr& value = problem.forms[form];
        for (std::size_t i = 0; i < plans.size(); ++i) {
            const State& state = plans[i].state;
            if (row_index(state.rows, value) >= 0) {
                targets.push_back({i, no_qubit, state.route_cost, state.hash});
                continue;
            }
            BitArr protected_rows;
            for (U32 required : forms) {
                const int row = row_index(state.rows, problem.forms[required]);
                if (row >= 0) protected_rows.set(static_cast<U16>(row));
            }
            const BitArr coordinates = gl::coordinates(value, state.inverse);
            const U32 cost = state.route_cost + static_cast<U32>(coordinates.count() - 1);
            for (U16 target = 0; target < problem.qubits; ++target) {
                if (coordinates.test(target) && !protected_rows.test(target)) {
                    targets.push_back({i, target, cost, load_hash(state, target, value)});
                }
            }
        }
        std::stable_sort(targets.begin(), targets.end(), [](const Target& a, const Target& b) {
            return std::tuple{a.cost, a.hash} < std::tuple{b.cost, b.hash};
        });
        std::vector<Plan> next;
        ankerl::unordered_dense::set<U64> seen;
        for (const Target& target : targets) {
            if (!seen.insert(target.hash).second) continue;
            Plan child = plans[target.parent];
            if (target.row != no_qubit) {
                State& state = child.state;
                BitArr coordinates = gl::coordinates(value, state.inverse);
                coordinates.flip(target.row);
                const BitArr identity = gl::unit<BitArr>(target.row);
                state.changed_rows -= state.rows[target.row] != identity;
                state.changed_rows += value != identity;
                state.rows[target.row] = value;
                for (BitArr& row : state.inverse) {
                    if (row.test(target.row)) row ^= coordinates;
                }
                state.route_cost = target.cost;
                state.hash = target.hash;
                child.loads[child.load_count++] = {form, target.row};
            }
            next.push_back(std::move(child));
            if (next.size() == target_width) break;
        }
        if (next.empty()) throw std::runtime_error("independent triple lost all targets");
        plans = std::move(next);
    }
    std::vector<State> result;
    for (Plan& plan : plans) {
        plan.state.done.set(term);
        plan.state.hash ^= term_hash(term);
        plan.state.trace = std::make_shared<Trace>(Trace{
            term, variant, plan.loads, plan.load_count, parent.trace});
        result.push_back(std::move(plan.state));
    }
    return result;
}

inline State find_route(const Problem& problem) {
    std::vector<State> beam{initial_state(problem)};
    for (std::size_t depth = 0; depth < problem.terms.size(); ++depth) {
        const std::size_t workers = std::min<std::size_t>(problem.workers, beam.size());
        std::vector<Beam> local;
        for (std::size_t i = 0; i < workers; ++i) local.emplace_back(problem.beam_width);
        gl::parallel_for(workers, problem.workers, [&](std::size_t worker) {
            for (std::size_t parent = worker; parent < beam.size(); parent += workers) {
                for (U16 term = 0; term < problem.terms.size(); ++term) {
                    if (beam[parent].done.test(term)) continue;
                    U64 order = target_width * (parent * problem.terms.size() + term);
                    for (State& child : complete_term(beam[parent], problem, term)) {
                        child.order = order++;
                        local[worker].add(std::move(child));
                    }
                }
            }
        });
        Beam next(problem.beam_width);
        for (Beam& group : local) {
            for (State& state : group.states) next.add(std::move(state));
        }
        if (next.states.empty()) throw std::runtime_error("cpr_xxx beam is empty");
        beam = std::move(next.states);
    }
    std::vector<U64> costs(beam.size());
    gl::parallel_for(beam.size(), problem.workers, [&](std::size_t i) {
        costs[i] = beam[i].route_cost + gl::estimate<BitArr>(beam[i].rows);
    });
    std::size_t selected = 0;
    for (std::size_t i = 1; i < beam.size(); ++i) {
        if (std::tuple{costs[i], beam[i].route_cost, beam[i].hash} <
            std::tuple{costs[selected], beam[selected].route_cost, beam[selected].hash}) {
            selected = i;
        }
    }
    return std::move(beam[selected]);
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

inline LowerPhase lower_product(
    const BitArr& u, const BitArr& v, const BitArr& w, U16 qubits) {
    LowerPhase result(qubits);
    for (U16 i = 0; i < qubits; ++i) {
        if (u.test(i) && v.test(i) && w.test(i)) result.linear.set(i);
        for (U16 j = i + 1; j < qubits; ++j) {
            const bool coefficient =
                (u.test(i) && v.test(i) && w.test(j)) ^
                (u.test(i) && v.test(j) && w.test(i)) ^
                (u.test(j) && v.test(i) && w.test(i)) ^
                (u.test(i) && v.test(j) && w.test(j)) ^
                (u.test(j) && v.test(i) && w.test(j)) ^
                (u.test(j) && v.test(j) && w.test(i));
            if (coefficient) result.quadratic.set(edge_index(qubits, i, j));
        }
    }
    return result;
}

inline void load_gates(
    std::vector<BitArr>& rows, std::vector<BitArr>& inverse,
    const BitArr& form, U16 target, std::vector<U64>& circuit) {
    const BitArr coordinates = gl::coordinates(form, inverse);
    for (U16 control = 0; control < rows.size(); ++control) {
        if (control == target || !coordinates.test(control)) continue;
        rows[target] ^= rows[control];
        for (U16 row = 0; row < rows.size(); ++row) {
            if (gl::bit(inverse[row], target)) inverse[row].flip(control);
        }
        circuit.push_back(make_cnot(control, target));
    }
}


inline std::vector<U64> route_circuit(
    const Problem& problem, const State& route) {
    std::vector<const Trace*> steps;
    for (auto trace = route.trace; trace; trace = trace->previous) {
        steps.push_back(trace.get());
    }
    std::reverse(steps.begin(), steps.end());
    auto rows = gl::identity<BitArr>(problem.qubits), inverse = rows;
    BitVec done(problem.terms.size());
    std::vector<U64> circuit;
    U32 routed = 0;
    for (const Trace* step : steps) {
        if (done.test(step->term)) throw std::runtime_error("term emitted twice");
        for (U8 i = 0; i < step->load_count; ++i) {
            const Load load = step->loads[i];
            const std::size_t before = circuit.size();
            load_gates(rows, inverse, problem.forms[load.form], load.target, circuit);
            routed += static_cast<U32>(circuit.size() - before);
        }
        std::array<U16, 3> wires{};
        for (U8 slot = 0; slot < 3; ++slot) {
            const U32 form = problem.terms[step->term].forms[step->variant[slot] - 1];
            const int row = row_index(rows, problem.forms[form]);
            if (row < 0) throw std::runtime_error("selected CCZ form is absent");
            wires[slot] = static_cast<U16>(row);
        }
        circuit.push_back(make_ccz(wires[0], wires[1], wires[2]));
        done.set(step->term);
    }
    if (done.count() != problem.terms.size() || rows != route.rows ||
        inverse != route.inverse || routed != route.route_cost) {
        throw std::runtime_error("cpr_xxx trace replay mismatch");
    }
    auto cleanup = gl::synt(problem.qubits, rows, problem.workers);
    gl::reverse(cleanup);
    for (U64 gate : cleanup) {
        gl::apply(rows, gate);
        circuit.push_back(gate);
    }
    if (!gl::is_identity<BitArr>(rows)) throw std::runtime_error("cpr_xxx cleanup failed");
    return circuit;
}

enum class FreeKind { cnot_s, ccz_x };

struct FreeSite {
    FreeKind kind;
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

struct Analysis {
    LowerPhase correction;
    std::vector<FreeSite> free;
};

inline Analysis analyze(
    U16 qubits, std::span<const U64> circuit,
    const BitArr& target_linear, const BitVec& target_quadratic,
    unsigned polarity_slots = 3) {
    const std::size_t quadratic_bits =
        static_cast<std::size_t>(qubits) * (qubits - 1) / 2;
    if (!gl::in_range(target_linear, qubits) ||
        target_quadratic.word_count() != BitVec(quadratic_bits).word_count()) {
        throw std::invalid_argument("invalid lower-phase dimensions");
    }
    Analysis result{LowerPhase(qubits), {}};
    result.correction.linear = target_linear;
    result.correction.quadratic = target_quadratic;
    std::vector<BitArr> rows = gl::identity<BitArr>(qubits);
    for (std::size_t index = 0; index < circuit.size(); ++index) {
        const U64 gate = circuit[index];
        if (opcode(gate) == cnot) {
            result.free.push_back(FreeSite{
                FreeKind::cnot_s, index, 0,
                product(rows[qubit(gate, 0)], rows[qubit(gate, 1)], qubits)});
            gl::apply(rows, gate);
        } else if (opcode(gate) == ccz) {
            const std::array<U16, 3> wires{
                qubit(gate, 0), qubit(gate, 1), qubit(gate, 2)};
            result.correction ^= lower_product(
                rows[wires[0]], rows[wires[1]], rows[wires[2]], qubits);
            for (U8 slot = 0; slot < std::min<unsigned>(3, polarity_slots); ++slot) {
                result.free.push_back(FreeSite{
                    FreeKind::ccz_x, index, slot,
                    product(rows[wires[(slot + 1) % 3]],
                            rows[wires[(slot + 2) % 3]], qubits)});
            }
        }
    }
    if (!gl::is_identity<BitArr>(rows)) {
        throw std::runtime_error("QPR requires a closed CNOT route");
    }
    return result;
}

inline ChargedSite charged_site(
    U16 qubits, std::span<const BitArr> rows,
    std::size_t boundary, U16 a, U16 b) {
    if (a > b) std::swap(a, b);
    return ChargedSite{
        boundary, a, b, product(rows[a], rows[b], qubits)};
}

inline void append_unique(
    std::vector<ChargedSite>& sites,
    ankerl::unordered_dense::set<U64>& seen, ChargedSite site) {
    const U64 hash = ankerl::unordered_dense::hash<BitVec>{}(
        site.phase.quadratic);
    if (seen.insert(hash).second) sites.push_back(std::move(site));
}

inline BitVec coordinate_mask(U16 qubits) {
    const std::size_t bits =
        static_cast<std::size_t>(qubits) * (qubits - 1) / 2;
    BitVec result(bits);
    for (std::size_t bit = 0; bit < bits; ++bit) result.set(bit);
    return result;
}

inline std::vector<ChargedSite> path_dictionary(
    U16 qubits, std::span<const U64> circuit, bool use_path_forms) {
    std::vector<ChargedSite> result;
    if (!use_path_forms) return result;

    ankerl::unordered_dense::set<U64> seen;
    const std::size_t quadratic_bits =
        static_cast<std::size_t>(qubits) * (qubits - 1) / 2;
    for (std::size_t bit = 0; bit < quadratic_bits; ++bit) {
        BitVec monomial(quadratic_bits);
        monomial.set(bit);
        seen.insert(ankerl::unordered_dense::hash<BitVec>{}(monomial));
    }

    std::vector<BitArr> rows = gl::identity<BitArr>(qubits);
    for (std::size_t index = 0; index < circuit.size(); ++index) {
        const U64 gate = circuit[index];
        if (opcode(gate) != cnot) continue;
        gl::apply(rows, gate);
        const U16 target = qubit(gate, 1);
        for (U16 wire = 0; wire < qubits; ++wire) {
            if (wire == target) continue;
            append_unique(result, seen, charged_site(
                qubits, rows, index + 1, target, wire));
        }
    }
    return result;
}

inline std::vector<U64> apply_qpr(
    U16 qubits, std::span<const U64> base,
    const BitArr& target_linear, const BitVec& target_quadratic,
    bool use_path_forms) {
    const Analysis analysis = analyze(
        qubits, base, target_linear, target_quadratic);
    const std::vector<ChargedSite> path = path_dictionary(
        qubits, base, use_path_forms);

    std::vector<BitVec> free_forms;
    free_forms.reserve(analysis.free.size());
    for (const FreeSite& site : analysis.free) {
        free_forms.push_back(site.phase.quadratic);
    }
    std::vector<BitVec> path_forms;
    path_forms.reserve(path.size());
    for (const ChargedSite& site : path) {
        path_forms.push_back(site.phase.quadratic);
    }
    const qpr::Solution solution = qpr::solve(
        analysis.correction.quadratic, free_forms, path_forms,
        coordinate_mask(qubits));

    std::vector<std::vector<std::size_t>> at(base.size() + 1);
    for (std::size_t i = 0; i < path.size(); ++i) {
        if (solution.charged.test(i)) at[path[i].boundary].push_back(i);
    }
    std::vector<int> s_source(base.size(), -1);
    std::vector<std::array<int, 3>> x_source(base.size(), {-1, -1, -1});
    for (std::size_t i = 0; i < analysis.free.size(); ++i) {
        const FreeSite& site = analysis.free[i];
        if (site.kind == FreeKind::cnot_s) s_source[site.gate] = static_cast<int>(i);
        else x_source[site.gate][site.slot] = static_cast<int>(i);
    }

    BitArr linear = analysis.correction.linear;
    for (std::size_t i = 0; i < analysis.free.size(); ++i) {
        if (solution.free.test(i)) linear ^= analysis.free[i].phase.linear;
    }
    for (std::size_t i = 0; i < path.size(); ++i) {
        if (solution.charged.test(i)) linear ^= path[i].phase.linear;
    }

    std::vector<U64> result;
    for (std::size_t index = 0; index <= base.size(); ++index) {
        for (const std::size_t selected : at[index]) {
            result.push_back(make_cz(path[selected].a, path[selected].b));
        }
        if (index == base.size()) {
            for (U16 a = 0; a < qubits; ++a) {
                for (U16 b = a + 1; b < qubits; ++b) {
                    if (solution.coordinate.test(edge_index(qubits, a, b))) {
                        result.push_back(make_cz(a, b));
                    }
                }
            }
            break;
        }
        const U64 gate = base[index];
        if (opcode(gate) == cnot && s_source[index] >= 0 &&
            solution.free.test(static_cast<std::size_t>(s_source[index]))) {
            const U16 control = qubit(gate, 0);
            const U16 target = qubit(gate, 1);
            result.push_back(make_s(control));
            result.push_back(make_s(target));
            result.push_back(gate);
            result.push_back(make_sdg(target));
        } else if (opcode(gate) == ccz) {
            const std::array<U16, 3> wires{
                qubit(gate, 0), qubit(gate, 1), qubit(gate, 2)};
            U8 mask = 0;
            for (U8 slot = 0; slot < 3; ++slot) {
                const int source = x_source[index][slot];
                if (source >= 0 && solution.free.test(static_cast<std::size_t>(source))) {
                    mask |= U8{1} << slot;
                    result.push_back(make_x(wires[slot]));
                }
            }
            result.push_back(gate);
            for (U8 slot = 3; slot-- > 0;) {
                if (mask & (U8{1} << slot)) result.push_back(make_x(wires[slot]));
            }
        } else {
            result.push_back(gate);
        }
    }

    // Pairwise complemented CCZ legs add the remaining linear parity.
    std::vector<BitArr> rows = gl::identity<BitArr>(qubits);
    for (std::size_t index = 0; index < base.size(); ++index) {
        const U64 gate = base[index];
        if (opcode(gate) == cnot) {
            gl::apply(rows, gate);
            continue;
        }
        if (opcode(gate) != ccz) continue;
        const std::array<U16, 3> wires{
            qubit(gate, 0), qubit(gate, 1), qubit(gate, 2)};
        U8 mask = 0;
        for (U8 slot = 0; slot < 3; ++slot) {
            const int source = x_source[index][slot];
            if (source >= 0 && solution.free.test(static_cast<std::size_t>(source))) {
                mask |= U8{1} << slot;
            }
        }
        if ((mask & 3) == 3) linear ^= rows[wires[2]];
        if ((mask & 5) == 5) linear ^= rows[wires[1]];
        if ((mask & 6) == 6) linear ^= rows[wires[0]];
    }
    for (U16 wire = 0; wire < qubits; ++wire) {
        if (linear.test(wire)) result.push_back(make_z(wire));
    }
    return result;
}

inline std::vector<U64> synt(
    U16 qubits, std::span<const BitArr> u,
    std::span<const BitArr> v, std::span<const BitArr> w,
    std::size_t beam_width, U16 workers = 1,
    bool use_path_forms = true) {
    const BitArr linear;
    const BitVec quadratic(
        static_cast<std::size_t>(qubits) * (qubits - 1) / 2);
    const Problem problem = make_problem(
        qubits, u, v, w, beam_width, workers);
    const State route = find_route(problem);
    return apply_qpr(
        qubits, route_circuit(problem, route), linear, quadratic,
        use_path_forms);
}

inline std::vector<U64> synt(
    U16 qubits, std::span<const BitArr> u,
    std::span<const BitArr> v, std::span<const BitArr> w,
    const BitArr& linear, const BitVec& quadratic,
    std::size_t beam_width, U16 workers = 1,
    bool use_path_forms = true) {
    const Problem problem = make_problem(
        qubits, u, v, w, beam_width, workers);
    const State route = find_route(problem);
    return apply_qpr(
        qubits, route_circuit(problem, route), linear, quadratic,
        use_path_forms);
}

} // namespace cnot::cpr_xxx
