#pragma once

#include "benchmark/benchmark.h"
#include "ctmd/linalg/inv.hpp"
#include "ctmd/random/rand.hpp"

namespace md = ctmd;

constexpr size_t RANGE_START = 1;
constexpr size_t RANGE_END = 10;
constexpr size_t RANGE_INCREMENT = 1;

template <typename T> inline void test(benchmark::State &state) noexcept {
    const size_t set_num = state.range(0);

    const auto in =
        md::random::rand<T, md::dims<2>>(md::dims<2>{set_num, set_num});
    auto out = md::mdarray<T, md::dims<2>>{md::dims<2>{set_num, set_num}};

    for (auto _ : state) {
        md::linalg::inv(in, out);
    }

    state.SetComplexityN(state.range(0));
}
