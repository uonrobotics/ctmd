<div align="center">
  <img src="logo.png" width="800" alt="Compile-Time Multi-Dimensional Matrix Library" />
</div>

## Overview
**CTMD(Compile-Time Multi-Dimensional matrix library)** is a header-only C++ library for multi-dimensional matrix operations and broadcasting. Built on C++23’s [std::mdspan](https://en.cppreference.com/w/cpp/container/mdspan) and C++26's [std::mdarray](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1684r2.html) and [std::submdspan](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2630r4.html), it enables both compile-time and run-time matrix computations. CTMD also supports acceleration through [Eigen](https://eigen.tuxfamily.org/) and [OpenMP](https://www.openmp.org/).

### Key Features
- **NumPy-like Syntax**: Designed with a familiar NumPy-style API for intuitive usage.
- **Compile-Time Computation**: Enables compile-time operations when possible for maximum efficiency.
- **Flexibility**: Seamlessly supports constexpr and non-constexpr contexts, diverse data types, and both static and dynamic extents.
- **High Performance**: Optimized for fast execution, leveraging modern C++ and hardware acceleration.
- **Modern C++ Support**: Fully compatible with C++23 and ready to adopt future features like [std::linalg](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1673r12.html) in C++26.

### Important Notes
- C++ Version Compatibility:
  - CTMD requires **C++23** for full functionality.
  - [std::mdspan](https://en.cppreference.com/w/cpp/container/mdspan) (C++23), [std::mdarray](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1684r2.html) (C++26), [std::submdspan](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2630r4.html) (C++26) are sourced from the [Reference mdspan implementation](https://github.com/kokkos/mdspan).
- Extent Conventions:
  - **Scalar (0D mdspan)**: Data stored as an **arithmetic type** (e.g., int, float, double).
  - **Vector (1D mdspan)**: Data stored as a **1D mdarray**.
  - **Matrix (2D mdspan)**: Data stored as a **2D mdarray**.
- Acceleration:
  - Using **Eigen**: Define the macro "USE_EIGEN" during build.
  - Using **OpenMP**: Add the -fopenmp flag during compilation and link with the OpenMP library (-lgomp).

### Available Functions
- **Array creation routines**: [empty](ctmd/empty.hpp), [empty_like](ctmd/empty_like.hpp), [eye](ctmd/eye.hpp), [ones](ctmd/ones.hpp), [ones_like](ctmd/ones_like.hpp), [zeros](ctmd/zeros.hpp), [zeros_like](ctmd/zeros_like.hpp), [full](ctmd/full.hpp), [full_like](ctmd/full_like.hpp), [copy](ctmd/copy.hpp), [arange](ctmd/arange.hpp), [linspace](ctmd/linspace.hpp).
- **Array manipulation routines**: [reshape](ctmd/reshape.hpp), [expand_dims](ctmd/expand_dims.hpp), [concatenate](ctmd/concatenate.hpp).
- **Linear algebra**: [matmul](ctmd/linalg/matmul.hpp), [matvec](ctmd/linalg/matvec.hpp), [vecmat](ctmd/linalg/vecmat.hpp), [norm](ctmd/linalg/norm.hpp), [inv](ctmd/linalg/inv.hpp).
- **Logic functions**: [all](ctmd/all.hpp), [any](ctmd/any.hpp), [allclose](ctmd/allclose.hpp), [isclose](ctmd/isclose.hpp), [array_equal](ctmd/array_equal.hpp), [array_equiv](ctmd/array_equiv.hpp), [greater](ctmd/greater.hpp), [greater_equal](ctmd/greater_equal.hpp), [less](ctmd/less.hpp), [less_equal](ctmd/less_equal.hpp), [equal](ctmd/equal.hpp), [not_equal](ctmd/not_equal.hpp).
- **Mathematical functions**: [sin](ctmd/sin.hpp), [cos](ctmd/cos.hpp), [tan](ctmd/tan.hpp), [atan2](ctmd/atan2.hpp), [deg2rad](ctmd/deg2rad.hpp), [rad2deg](ctmd/rad2deg.hpp), [sum](ctmd/sum.hpp), [add](ctmd/add.hpp), [negative](ctmd/negative.hpp), [multiply](ctmd/multiply.hpp), [divide](ctmd/divide.hpp), [subtract](ctmd/subtract.hpp), [maximum](ctmd/maximum.hpp), [minimum](ctmd/minimum.hpp), [clip](ctmd/clip.hpp), [sqrt](ctmd/sqrt.hpp), [absolute](ctmd/absolute.hpp).
- **Random sampling**: [rand](ctmd/random/rand.hpp), [uniform](ctmd/random/uniform.hpp).

## Installation
CTMD is a header-only library, so you can start using it by simply including [ctmd/ctmd.hpp](ctmd/ctmd.hpp) in your project.

### Using Bazel
To integrate CTMD into a Bazel project, add it as a dependency in your WORKSPACE file. Ensure your compiler supports C++23. For specific setup requirements, refer to the [.devcontainer/DockerFile](.devcontainer/DockerFile) file. Alternatively, you can use the provided development container to automatically configure your environment.

## Examples

1. Matrix addition

    Code:
    ```cpp
    #include "ctmd/ctmd.hpp"

    namespace md = ctmd;

    constexpr auto a = md::full<int, md::extents<size_t, 3, 1, 2>>(1);
    constexpr auto b = md::full<int, md::extents<size_t, 2, 1>>(2);
    constexpr auto c = md::add(a, b);

    constexpr auto c_expect = md::full<int, md::extents<size_t, 3, 2, 2>>(3);

    constexpr bool is_allclose = md::allclose(c, c_expect);

    std::cout << "a extents: " << md::to_string(a.extents()) << std::endl;
    std::cout << "a: " << md::to_string(a) << std::endl << std::endl;

    std::cout << "b extents: " << md::to_string(b.extents()) << std::endl;
    std::cout << "b: " << md::to_string(b) << std::endl << std::endl;

    std::cout << "c extents: " << md::to_string(c.extents()) << std::endl;
    std::cout << "c: " << md::to_string(c) << std::endl;

    static_assert(is_allclose);
    ```

    Output:
    ```bash
    a extents: (3, 1, 2)
    a: [[[1, 1]], [[1, 1]], [[1, 1]]]

    b extents: (2, 1)
    b: [[2], [2]]

    c extents: (3, 2, 2)
    c: [[[3, 3], [3, 3]], [[3, 3], [3, 3]], [[3, 3], [3, 3]]]
    ```

## Tests and Benchmarks
CTMD uses [GoogleTest](https://github.com/google/googletest) for unit tests, located in the tests/ directory. Run all tests using Bazel:
```bash
bazel test tests/...
```

Benchmarks (in benchmarks/) are written using [GoogleBenchmark](https://github.com/google/benchmark). To run a benchmark (e.g., matrix addition in double precision):
```bash
bazel run benchmarks/add:pd
```

## About
This project is developed and maintained by [Uon robotics](https://uonrobotics.com/), South Korea.
As a robotics company, we are committed to building robust, efficient software — and we believe in the power of open-source collaboration.

## Contributing
CTMD is in active development, and contributions are welcome!
Feel free to open issues or pull requests — whether for bug reports, feature ideas, or code improvements.

## License
CTMD is licensed under the Apache 2.0 License. See the [LICENSE](LICENSE) file for full details.
