#pragma once

#include "benchmark/benchmark.h"
#include "ctmd/atan2.hpp"
#include "ctmd/random/rand.hpp"

namespace md = ctmd;

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 1e8;
constexpr size_t RANGE_MULTIPLIER = 10;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    const auto in1 = md::random::rand<T, md::dims<1>>(md::dims<1>{set_num});
    const auto in2 = md::random::rand<T, md::dims<1>>(md::dims<1>{set_num});
    auto out = md::mdarray<T, md::dims<1>>{md::dims<1>{set_num}};

    for (auto _ : state) {
        md::atan2_to(in1, in2, out);
    }

    state.SetComplexityN(state.range(0));
}
