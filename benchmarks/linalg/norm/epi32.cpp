#include "source.hpp"

using TARGET_TYPE = int32_t;

void bm(benchmark::State &state) { test<TARGET_TYPE>(state); }

BENCHMARK(bm)
    ->DenseRange(RANGE_START, RANGE_END, RANGE_INCREMENT)
    ->Unit(benchmark::kMicrosecond)
    ->Complexity(benchmark::oN);

BENCHMARK_MAIN();
