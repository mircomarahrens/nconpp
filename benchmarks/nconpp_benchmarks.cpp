// Copyright 2026 Mirco Marahrens

#include <benchmark/benchmark.h>

#include <complex>
#include <cstddef>
#include <utility>
#include <vector>

#include "nconpp/graph.hpp"
#include "nconpp/lattice_graph.hpp"
#include "nconpp/linalg.hpp"
#include "nconpp/tensor.hpp"
#include "nconpp/tensor_network.hpp"

namespace {

// Build a chain of N tensors (MPS-like) with bond dimension chi and physical dimension d.
auto make_mps_chain(std::size_t n_tensors, std::size_t chi, std::size_t d) {
    xt::random::seed(42);
    std::vector<npp::tensor_type<std::complex<double>>> tensors;
    std::vector<std::vector<int>> legs;
    tensors.reserve(n_tensors);
    legs.reserve(n_tensors);

    for (std::size_t i = 0; i < n_tensors; ++i) {
        int phys_leg = -static_cast<int>(i + 1);
        if (i == 0) {
            tensors.emplace_back(xt::random::rand<double>({d, chi}));
            legs.push_back({phys_leg, 1});
        } else if (i + 1 == n_tensors) {
            tensors.emplace_back(xt::random::rand<double>({chi, d}));
            legs.push_back({static_cast<int>(i), phys_leg});
        } else {
            tensors.emplace_back(xt::random::rand<double>({chi, d, chi}));
            legs.push_back({static_cast<int>(i), phys_leg, static_cast<int>(i + 1)});
        }
    }
    return std::make_pair(std::move(tensors), std::move(legs));
}

// Build a 2-row ladder of 2*cols tensors with parallel bonds after contraction.
auto make_ladder_network(std::size_t cols, std::size_t chi) {
    xt::random::seed(42);
    std::vector<npp::tensor_type<std::complex<double>>> tensors;
    std::vector<std::vector<int>> legs;
    tensors.reserve(2 * cols);
    legs.reserve(2 * cols);

    int next_leg = 1;
    std::vector<int> rung_legs(cols);
    for (std::size_t c = 0; c < cols; ++c) {
        rung_legs[c] = next_leg++;
    }
    std::vector<int> top_legs(cols - 1);
    std::vector<int> bot_legs(cols - 1);
    for (std::size_t c = 0; c + 1 < cols; ++c) {
        top_legs[c] = next_leg++;
        bot_legs[c] = next_leg++;
    }

    int dangling = -1;
    for (std::size_t c = 0; c < cols; ++c) {
        std::vector<int> t_legs;
        npp::shape_type t_shape;
        if (c > 0) {
            t_legs.push_back(top_legs[c - 1]);
            t_shape.push_back(chi);
        }
        t_legs.push_back(rung_legs[c]);
        t_shape.push_back(chi);
        if (c + 1 < cols) {
            t_legs.push_back(top_legs[c]);
            t_shape.push_back(chi);
        }
        if (c == 0 || c + 1 == cols) {
            t_legs.push_back(dangling--);
            t_shape.push_back(2);
        }
        tensors.emplace_back(xt::random::rand<double>(t_shape));
        legs.push_back(std::move(t_legs));

        std::vector<int> b_legs;
        npp::shape_type b_shape;
        if (c > 0) {
            b_legs.push_back(bot_legs[c - 1]);
            b_shape.push_back(chi);
        }
        b_legs.push_back(rung_legs[c]);
        b_shape.push_back(chi);
        if (c + 1 < cols) {
            b_legs.push_back(bot_legs[c]);
            b_shape.push_back(chi);
        }
        tensors.emplace_back(xt::random::rand<double>(b_shape));
        legs.push_back(std::move(b_legs));
    }

    return std::make_pair(std::move(tensors), std::move(legs));
}

// --- Graph ---

void BM_Graph_MergeVerticesChain(benchmark::State &state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    for (auto _ : state) {
        Graph<> g(n);
        for (std::size_t i = 0; i + 1 < n; ++i) {
            g.addEdge(static_cast<int>(i + 1), i, i + 1);
        }
        for (std::size_t i = 1; i < n; ++i) {
            g.removeEdge(static_cast<int>(i));
            g.mergeVertices(0, i);
        }
        benchmark::DoNotOptimize(g.NumVertices());
    }
}
BENCHMARK(BM_Graph_MergeVerticesChain)->Arg(200)->Unit(benchmark::kMicrosecond);

// --- LatticeGraph ---

void BM_LatticeGraph_InitHoneycomb32x32PBC(benchmark::State &state) {
    for (auto _ : state) {
        LatticeGraph lg("Honeycomb", {32, 32});
        benchmark::DoNotOptimize(lg.NumEdges());
    }
}
BENCHMARK(BM_LatticeGraph_InitHoneycomb32x32PBC)->Unit(benchmark::kMicrosecond);

void BM_LatticeGraph_RemoveEdgelessBoundary32x32OBC(benchmark::State &state) {
    for (auto _ : state) {
        state.PauseTiming();
        LatticeGraph lg("Honeycomb", {32, 32},
                        {{{0, -1}, {0, +1}, {+1, 0}}, {{-1, 0}, {0, -1}, {0, +1}}}, {"obc", "obc"});
        state.ResumeTiming();

        lg.removeEdgelessBoundaryVertices();
        benchmark::DoNotOptimize(lg.NumVertices());
    }
}
BENCHMARK(BM_LatticeGraph_RemoveEdgelessBoundary32x32OBC)->Unit(benchmark::kMicrosecond);

// --- Tensor / LinAlg ---

void BM_Tensor_TensordotContiguous(benchmark::State &state) {
    xt::random::seed(42);
    npp::tensor_type<std::complex<double>> a = xt::random::rand<double>({16, 16, 16});
    npp::tensor_type<std::complex<double>> b = xt::random::rand<double>({16, 16, 16});

    for (auto _ : state) {
        auto r = npp::linalg::tensordot(a, b, {2}, {0});
        benchmark::DoNotOptimize(r.data());
    }
}
BENCHMARK(BM_Tensor_TensordotContiguous)->Unit(benchmark::kMicrosecond);

void BM_Tensor_TensordotNonContiguous(benchmark::State &state) {
    xt::random::seed(42);
    npp::tensor_type<std::complex<double>> a = xt::random::rand<double>({16, 16, 16});
    npp::tensor_type<std::complex<double>> b = xt::random::rand<double>({16, 16, 16});

    for (auto _ : state) {
        auto r = npp::linalg::tensordot(a, b, {1}, {1});
        benchmark::DoNotOptimize(r.data());
    }
}
BENCHMARK(BM_Tensor_TensordotNonContiguous)->Unit(benchmark::kMicrosecond);

void BM_Tensor_Outer32x32(benchmark::State &state) {
    xt::random::seed(42);
    npp::tensor_type<std::complex<double>> a = xt::random::rand<double>({32, 32});
    npp::tensor_type<std::complex<double>> b = xt::random::rand<double>({32, 32});

    for (auto _ : state) {
        auto r = npp::linalg::outer(a, b);
        benchmark::DoNotOptimize(r.data());
    }
}
BENCHMARK(BM_Tensor_Outer32x32)->Unit(benchmark::kMicrosecond);

void BM_LinAlg_HOSVD_12x12x12x12(benchmark::State &state) {
    xt::random::seed(42);
    const auto rank = static_cast<std::size_t>(state.range(0));
    npp::tensor_type<double> t = xt::random::rand<double>({12, 12, 12, 12});

    for (auto _ : state) {
        auto [factors, core] = npp::linalg::hosvd(t, rank);
        benchmark::DoNotOptimize(core.data());
        benchmark::DoNotOptimize(factors.data());
    }
}
BENCHMARK(BM_LinAlg_HOSVD_12x12x12x12)->Arg(0)->Arg(6)->Unit(benchmark::kMicrosecond);

// --- TensorNetwork ---

void BM_TensorNetwork_GetLegs(benchmark::State &state) {
    auto [tensors, legs] = make_mps_chain(10, 16, 2);
    TensorNetwork<> tn(tensors, legs);

    for (auto _ : state) {
        auto m = tn.getLegs();
        benchmark::DoNotOptimize(m.size());
    }
}
BENCHMARK(BM_TensorNetwork_GetLegs)->Unit(benchmark::kMicrosecond);

void BM_TensorNetwork_ContractMPS(benchmark::State &state) {
    auto [tensors, legs] = make_mps_chain(10, 16, 2);
    TensorNetwork<> base_tn(tensors, legs);

    for (auto _ : state) {
        state.PauseTiming();
        TensorNetwork<> tn(base_tn);
        state.ResumeTiming();

        tn.contract();
        benchmark::DoNotOptimize(tn.NumTensors());
    }
}
BENCHMARK(BM_TensorNetwork_ContractMPS)->Unit(benchmark::kMicrosecond);

void BM_TensorNetwork_ContractLadder(benchmark::State &state) {
    const bool multi_index = state.range(0) != 0;
    auto [tensors, legs] = make_ladder_network(8, 8);
    TensorNetwork<> base_tn(tensors, legs);

    for (auto _ : state) {
        state.PauseTiming();
        TensorNetwork<> tn(base_tn);
        state.ResumeTiming();

        tn.contract({}, {}, multi_index);
        benchmark::DoNotOptimize(tn.NumTensors());
    }
}
BENCHMARK(BM_TensorNetwork_ContractLadder)->Arg(0)->Arg(1)->Unit(benchmark::kMicrosecond);

void BM_TensorNetwork_ContractWithSV(benchmark::State &state) {
    xt::random::seed(42);
    npp::tensor_type<std::complex<double>> a = xt::random::rand<double>({8, 16, 16});
    npp::tensor_type<std::complex<double>> b = xt::random::rand<double>({16, 16, 8});
    npp::tensor_type<std::complex<double>> sv1 = xt::random::rand<double>({16});
    npp::tensor_type<std::complex<double>> sv2 = xt::random::rand<double>({16});
    TensorNetwork<> base_tn({a, b}, {{-1, 1, 2}, {1, 2, -2}});
    base_tn.edges[1].singular_values = sv1;
    base_tn.edges[2].singular_values = sv2;

    for (auto _ : state) {
        state.PauseTiming();
        TensorNetwork<> tn(base_tn);
        state.ResumeTiming();

        tn.contract({}, {}, true);
        benchmark::DoNotOptimize(tn.NumTensors());
    }
}
BENCHMARK(BM_TensorNetwork_ContractWithSV)->Unit(benchmark::kMicrosecond);

void BM_TensorNetwork_Split(benchmark::State &state) {
    xt::random::seed(42);
    npp::tensor_type<std::complex<double>> t = xt::random::rand<double>({8, 8, 8, 8});
    TensorNetwork<> base_tn({t}, {{-1, -2, -3, -4}});

    for (auto _ : state) {
        state.PauseTiming();
        TensorNetwork<> tn(base_tn);
        state.ResumeTiming();

        tn.split(0, 2);
        benchmark::DoNotOptimize(tn.NumTensors());
    }
}
BENCHMARK(BM_TensorNetwork_Split)->Unit(benchmark::kMicrosecond);

} // namespace
