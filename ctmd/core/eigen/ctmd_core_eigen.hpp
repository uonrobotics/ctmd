#pragma once

#ifdef USE_EIGEN

#include <Eigen/Dense>

#include "../ctmd_core_convert.hpp"
#include "../ctmd_core_type.hpp"

namespace ctmd {
namespace core {
namespace eigen {
namespace detail {

[[nodiscard]] inline constexpr int ext_to_dyn(const size_t extent) noexcept {
    if (extent == dyn) {
        return Eigen::Dynamic;

    } else if (extent < std::numeric_limits<int>::max()) {
        return static_cast<int>(extent);

    } else {
        assert(false);
        return Eigen::Dynamic;
    }
}

} // namespace detail

template <typename T>
concept eigen_mappable_mdspan_c =
    (mdspan_c<T> &&
     (std::is_same_v<typename T::layout_type, layout_right> ||
      std::is_same_v<typename T::layout_type, layout_left>) &&
     T::rank() == 2 && T::is_always_unique() && T::is_always_exhaustive() &&
     T::is_always_strided());

template <eigen_mappable_mdspan_c in_t>
[[nodiscard]] inline constexpr auto to_eigen(const in_t &in) noexcept {
    const auto in_span = core::to_mdspan(in);
    using in_span_t = decltype(in_span);

    using Scalar = typename in_span_t::element_type;
    using Lay = typename in_span_t::layout_type;

    constexpr int RowsAtCompileTime =
        detail::ext_to_dyn(in_span_t::static_extent(0));

    constexpr int ColsAtCompileTime =
        detail::ext_to_dyn(in_span_t::static_extent(1));

    constexpr auto Options = []() {
        if constexpr (ColsAtCompileTime == 1 && RowsAtCompileTime != 1) {
            return Eigen::ColMajor;

        } else if constexpr (std::is_same_v<Lay, layout_right>) {
            return Eigen::RowMajor;

        } else if constexpr (std::is_same_v<Lay, layout_left>) {
            return Eigen::ColMajor;

        } else {
            static_assert(false, "Invalid layout type for Eigen mapping");
        }
    }();

    const int Rows = detail::ext_to_dyn(in_span.extent(0));
    const int Cols = detail::ext_to_dyn(in_span.extent(1));

    using eigen_t = std::conditional_t<
        std::is_const_v<Scalar>,
        const Eigen::Matrix<std::remove_const_t<Scalar>, RowsAtCompileTime,
                            ColsAtCompileTime, Options>,
        Eigen::Matrix<Scalar, RowsAtCompileTime, ColsAtCompileTime, Options>>;

    return Eigen::Map<eigen_t>{in_span.data_handle(), Rows, Cols};
}

} // namespace eigen
} // namespace core
} // namespace ctmd

#endif
