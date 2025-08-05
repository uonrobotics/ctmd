#include "source.hpp"

using TARGET_TYPE = uint16_t;

void bm(benchmark::State &state) { test<TARGET_TYPE>(state); }

BENCHMARK(bm)
    ->RangeMultiplier(RANGE_MULTIPLIER)
    ->Range(RANGE_START, RANGE_END)
    ->Unit(benchmark::kMicrosecond)
    ->Complexity(benchmark::oN);

BENCHMARK_MAIN();
