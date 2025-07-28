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
- **Array creation routines**: [empty](ctmd/ctmd_empty.hpp), [empty_like](ctmd/ctmd_empty_like.hpp), [eye](ctmd/ctmd_eye.hpp), [ones](ctmd/ctmd_ones.hpp), [ones_like](ctmd/ctmd_ones_like.hpp), [zeros](ctmd/ctmd_zeros.hpp), [zeros_like](ctmd/ctmd_zeros_like.hpp), [full](ctmd/ctmd_full.hpp), [full_like](ctmd/ctmd_full_like.hpp), [copy](ctmd/ctmd_copy.hpp), [arange](ctmd/ctmd_arange.hpp), [linspace](ctmd/ctmd_linspace.hpp).
- **Array manipulation routines**: [reshape](ctmd/ctmd_reshape.hpp), [expand_dims](ctmd/ctmd_expand_dims.hpp), [concatenate](ctmd/ctmd_concatenate.hpp).
- **Linear algebra**: [matmul](ctmd/linalg/ctmd_linalg_matmul.hpp), [matvec](ctmd/linalg/ctmd_linalg_matvec.hpp), [vecmat](ctmd/linalg/ctmd_linalg_vecmat.hpp), [norm](ctmd/linalg/ctmd_linalg_norm.hpp), [inv](ctmd/linalg/ctmd_linalg_inv.hpp).
- **Logic functions**: [all](ctmd/ctmd_all.hpp), [any](ctmd/ctmd_any.hpp), [allclose](ctmd/ctmd_allclose.hpp), [isclose](ctmd/ctmd_isclose.hpp), [array_equal](ctmd/ctmd_array_equal.hpp), [array_equiv](ctmd/ctmd_array_equiv.hpp), [greater](ctmd/ctmd_greater.hpp), [greater_equal](ctmd/ctmd_greater_equal.hpp), [less](ctmd/ctmd_less.hpp), [less_equal](ctmd/ctmd_less_equal.hpp), [equal](ctmd/ctmd_equal.hpp), [not_equal](ctmd/ctmd_not_equal.hpp).
- **Mathematical functions**: [sin](ctmd/ctmd_sin.hpp), [cos](ctmd/ctmd_cos.hpp), [tan](ctmd/ctmd_tan.hpp), [atan2](ctmd/ctmd_atan2.hpp), [deg2rad](ctmd/ctmd_deg2rad.hpp), [rad2deg](ctmd/ctmd_rad2deg.hpp), [sum](ctmd/ctmd_sum.hpp), [add](ctmd/ctmd_add.hpp), [negative](ctmd/ctmd_negative.hpp), [multiply](ctmd/ctmd_multiply.hpp), [divide](ctmd/ctmd_divide.hpp), [subtract](ctmd/ctmd_subtract.hpp), [maximum](ctmd/ctmd_maximum.hpp), [minimum](ctmd/ctmd_minimum.hpp), [clip](ctmd/ctmd_clip.hpp), [sqrt](ctmd/ctmd_sqrt.hpp), [absolute](ctmd/ctmd_absolute.hpp), [sign](ctmd/ctmd_sign.hpp).
- **Random sampling**: [rand](ctmd/random/ctmd_random_rand.hpp), [uniform](ctmd/random/ctmd_random_uniform.hpp).

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
bazel run benchmarks/add/none:ps
```

## About
This project is developed and maintained by [Uon robotics](https://uonrobotics.com/), South Korea.
As a robotics company, we are committed to building robust, efficient software — and we believe in the power of open-source collaboration.

## Contributing
CTMD is in active development, and contributions are welcome!
Feel free to open issues or pull requests — whether for bug reports, feature ideas, or code improvements.

## License
CTMD is licensed under the Apache 2.0 License. See the [LICENSE](LICENSE) file for full details.
