#pragma once

#include "benchmark/benchmark.h"
#include "ctmd/ctmd_full.hpp"
#include "ctmd/ctmd_multiply.hpp"

namespace md = ctmd;

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 1e8;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    const auto in1 = md::full<T, md::dims<1>>(1, md::dims<1>{set_num});
    const auto in2 = md::full<T, md::dims<1>>(2, md::dims<1>{set_num});
    auto out = md::mdarray<T, md::dims<1>>{md::dims<1>{set_num}};

    for (auto _ : state) {
        md::multiply_to(in1, in2, out);
    }

    state.SetComplexityN(state.range(0));
}
