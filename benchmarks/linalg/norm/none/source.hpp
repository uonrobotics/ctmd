#pragma once

#include "benchmark/benchmark.h"
#include "ctmd/ctmd_full.hpp"
#include "ctmd/linalg/ctmd_linalg_norm.hpp"

namespace md = ctmd;

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 10;
constexpr size_t RANGE_INCREMENT = 1;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    const auto in = md::full<T, md::dims<1>>(1, md::dims<1>{set_num});
    T out;

    for (auto _ : state) {
        md::linalg::norm_to(in, out);
    }

    state.SetComplexityN(state.range(0));
}
