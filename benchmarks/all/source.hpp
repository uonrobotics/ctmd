#pragma once

#include "benchmark/benchmark.h"
#include "ctmd/all.hpp"
#include "ctmd/full.hpp"

namespace md = ctmd;

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 1e8;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    const auto in = md::full<T, md::dims<1>>(true, md::dims<1>{set_num});

    for (auto _ : state) {
        const auto out = md::all(in);
        benchmark::DoNotOptimize(out);
    }

    state.SetComplexityN(state.range(0));
}
