#include "source.hpp"

using TARGET_TYPE = int32_t;
constexpr bool MULTI_PROCESS = true;

void bm(benchmark::State &state) { test<TARGET_TYPE>(state, MULTI_PROCESS); }

BENCHMARK(bm)
    ->RangeMultiplier(RANGE_MULTIPLIER)
    ->Range(RANGE_START, RANGE_END)
    ->Unit(benchmark::kMicrosecond)
    ->Complexity(benchmark::oN);

BENCHMARK_MAIN();
