<center>
<span style="color:rgb(0, 188, 255); font-size:200%;">C</span><span style="color:rgb(100, 100, 100); font-size:200%;">ompile-</span>
<span style="color:rgb(0, 188, 255); font-size:200%;">T</span><span style="color:rgb(100, 100, 100); font-size:200%;">ime</span>
<span style="color:rgb(0, 188, 255); font-size:200%;">M</span><span style="color:rgb(100, 100, 100); font-size:200%;">ulti-</span>
<span style="color:rgb(0, 188, 255); font-size:200%;">D</span><span style="color:rgb(100, 100, 100); font-size:200%;">imensional Matrix Library</span>
</center>

## Overview
CTMD(Compile-Time Multi-Dimensional matrix library) is a hearder-only library for multi-dimensional matrix operations and broadcasting, written in C++. This library uses C++23's [std::mdspan](https://en.cppreference.com/w/cpp/container/mdspan) and [std::mdarray](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1684r2.html) to represent and compute multi-dimensional matrices. It also supports accelerated operations through [Eigen](https://eigen.tuxfamily.org/) and [OpenMP](https://www.openmp.org/).

## Installation
CTMD is a header-only library. It can be easily integrated into your projects by #include [ctmd/ctmd.hpp](ctmd/ctmd.hpp).
If you are using Bazel, you can also import CMTD as a dependency in your WORKSPACE file.
CTMD needs C++23 support. Other requirements can be found in the [.devcontainer/postCreateCommand.sh](.devcontainer/postCreateCommand.sh) file.
Or you can use devcontainer to set up the development environment automatically.

## Examples
Simple example of matrix addition:
```cpp
#include "ctmd/ctmd.hpp"

namespace md = ctmd;

constexpr auto a = md::full<int, md::extents<size_t, 3, 1, 2>>(1);
constexpr auto b = md::full<int, md::extents<size_t, 2, 1>>(2);
constexpr auto c = md::add(a, b);

constexpr auto c_expect = md::full<int, md::extents<size_t, 3, 2, 2>>(3);

constexpr bool is_allclose = md::allclose(c, c_expect);

std::cout << "a extents: " << md::to_string(a.extents()) << std::endl;
std::cout << "a: " << md::to_string(a) << std::endl;
std::cout << "b extents: " << md::to_string(b.extents()) << std::endl;
std::cout << "b: " << md::to_string(b) << std::endl;
std::cout << "c extents: " << md::to_string(c.extents()) << std::endl;
std::cout << "c: " << md::to_string(c) << std::endl;

static_assert(is_allclose);
```

results:
```bash
a extents: (3, 1, 2)
a: [[[1, 1]], [[1, 1]], [[1, 1]]]
b extents: (2, 1)
b: [[2], [2]]
c extents: (3, 2, 2)
c: [[[3, 3], [3, 3]], [[3, 3], [3, 3]], [[3, 3], [3, 3]]]
```

## Testing
Test all with bazel:
```bash
bazel test ...
```

