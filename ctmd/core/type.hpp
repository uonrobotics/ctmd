#pragma once

#include <experimental/mdarray>
#include <experimental/mdspan> // TODO: Remove when C++23 std::mdspan supports

namespace std::experimental {

// dims: will be included in C++23
// (https://en.cppreference.com/w/cpp/container/mdspan/extents)
template <size_t Rank, class IndexType = size_t>
using dims = dextents<IndexType, Rank>;

static_assert(dynamic_extent == std::dynamic_extent);

_MDSPAN_INLINE_VARIABLE constexpr auto dyn = dynamic_extent;

} // namespace std::experimental

namespace ctmd {

using namespace std::experimental;

#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#define REAL_GCC __GNUC__ // probably
#endif

namespace detail {

template <typename T> struct make_extents_impl {
    template <size_t... Is>
    static constexpr auto generate(std::index_sequence<Is...>) noexcept {
        return std::experimental::extents<typename T::index_type,
                                          T::static_extent(Is)...>{};
    }
};

} // namespace detail

template <typename T>
concept extents_c =
    requires {
        typename T::index_type;
        typename T::size_type;
        typename T::rank_type;
    } &&
    (std::is_same_v<T, decltype(detail::make_extents_impl<T>::generate(
                           std::make_index_sequence<T::rank()>{}))> ||
     std::is_same_v<T, const decltype(detail::make_extents_impl<T>::generate(
                           std::make_index_sequence<T::rank()>{}))>);

template <typename T>
concept arithmetic_c = std::is_arithmetic_v<T>;

template <typename T>
concept floating_point_c = std::is_floating_point_v<T>;

// NOTE: mdspan_c is mainly targeted for C++23 std::mdspan, but it can also be
// used with other mdspan-like implementation that follow the same interface.
template <typename T>
concept mdspan_c = requires {
    typename T::element_type;
    typename T::extents_type;
    typename T::layout_type;
    typename T::accessor_type;
};

template <extents_c exts_t>
[[nodiscard]] inline constexpr size_t
static_size(const exts_t &exts = exts_t{}) noexcept {
    if constexpr (exts_t::rank() == 0) {
        return 0;

    } else if constexpr (exts_t::rank_dynamic() == 0) {
        return []<size_t... Is>(std::index_sequence<Is...>) {
            return (exts_t::static_extent(Is) * ...);
        }(std::make_index_sequence<exts_t::rank()>{});

    } else {
        return dyn;
    }
}

template <extents_c exts_t>
[[nodiscard]] inline constexpr size_t
size(const exts_t &exts = exts_t{}) noexcept {
    if constexpr (exts_t::rank() == 0) {
        return 0;

    } else if constexpr (exts_t::rank_dynamic() == 0) {
        return []<size_t... Is>(std::index_sequence<Is...>) {
            return (exts_t::static_extent(Is) * ...);
        }(std::make_index_sequence<exts_t::rank()>{});

    } else {
        return [&exts]<size_t... Is>(std::index_sequence<Is...>) {
            return (exts.extent(Is) * ...);
        }(std::make_index_sequence<exts_t::rank()>{});
    }
}

template <extents_c in_t>
[[nodiscard]] inline constexpr bool same(const in_t &in = in_t{}) noexcept {
    return true;
}

template <extents_c in1_t, extents_c in2_t, extents_c... ins_t>
[[nodiscard]] inline constexpr bool same(const in1_t &in1 = in1_t{},
                                         const in2_t &in2 = in2_t{},
                                         const ins_t &...ins) noexcept {
    if constexpr (in1_t::rank() != in2_t::rank()) {
        return false;

    } else if constexpr (in1_t::rank_dynamic() == 0 &&
                         in2_t::rank_dynamic() == 0 &&
                         []<size_t... Is>(std::index_sequence<Is...>) {
                             return ((in1_t::static_extent(Is) !=
                                      in2_t::static_extent(Is)) ||
                                     ...);
                         }(std::make_index_sequence<in1_t::rank()>{})) {
        return false;

    } else {
        for (size_t i = 0; i < in1_t::rank(); i++) {
            if (in1.extent(i) != in2.extent(i)) {
                return false;
            }
        }
    }

    if constexpr (sizeof...(ins_t) != 0) {
        return same(in2, ins...);

    } else {
        return true;
    }
}

template <typename T, extents_c extent_t>
using mdarray = std::conditional_t<
    extent_t::rank_dynamic() == 0,
    std::experimental::mdarray<T, extent_t, layout_right,
                               std::array<T, static_size<extent_t>()>>,
    std::experimental::mdarray<T, extent_t, layout_right, std::vector<T>>>;

template <size_t start, size_t end>
using slice = std::experimental::strided_slice<
    std::integral_constant<size_t, start>,
    std::integral_constant<size_t, end - start>,
    std::integral_constant<size_t, 1>>;

enum class MPMode : uint8_t {
    NONE,
    SIMD, // NOTE: Testing
    CPUMP,
};

template <typename T> struct element_type {
    using type = T;
};

template <typename T, typename Extents, typename Layout, typename Accessor>
struct element_type<std::experimental::mdspan<T, Extents, Layout, Accessor>> {
    using type = T;
};

template <typename T, typename Extents, typename Layout, typename Container>
struct element_type<std::experimental::mdarray<T, Extents, Layout, Container>> {
    using type = T;
};

template <typename T>
using element_type_t = typename element_type<std::remove_cvref_t<T>>::type;

} // namespace ctmd
