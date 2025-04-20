#pragma once

#include "benchmark/benchmark.h"
#include "ctmd/ctmd.hpp"

namespace md = ctmd;

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 1e10;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T>
inline void test(benchmark::State &state, const bool multi_process) noexcept {
    const size_t set_num = state.range(0);

#if false
    const auto a = md::full<T, md::dims<1>>(1, md::dims<1>{set_num});
    const auto b = md::full<T, md::dims<1>>(2, md::dims<1>{set_num});
    auto c = md::mdarray<T, md::dims<1>>{md::dims<1>{set_num}};

    for (auto _ : state) {
        md::add(a, b, c, multi_process);
    }
    benchmark::DoNotOptimize(c);

#else

    auto a = std::vector<T>{};
    auto b = std::vector<T>{};
    auto c = std::vector<T>{};

    a.resize(set_num, 1);
    b.resize(set_num, 2);
    c.resize(set_num);

    T *pa = a.data();
    T *pb = b.data();
    T *pc = c.data();

    for (auto _ : state) {
#pragma acc parallel loop
        for (size_t i = 0; i < set_num; ++i) {
            pc[i] = pa[i] + pb[i];
        }
    }
    benchmark::DoNotOptimize(c);

#endif

    state.SetComplexityN(state.range(0));
}
