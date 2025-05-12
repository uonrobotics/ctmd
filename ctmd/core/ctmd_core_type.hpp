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
concept mdspan_c =
    requires {
        typename T::element_type;
        typename T::extents_type;
        typename T::layout_type;
        typename T::accessor_type;
    } &&
    std::is_same_v<std::remove_const_t<T>,
                   std::experimental::mdspan<
                       typename T::element_type, typename T::extents_type,
                       typename T::layout_type, typename T::accessor_type>>;

template <typename T>
concept mdarray_c =
    requires {
        typename T::element_type;
        typename T::extents_type;
        typename T::layout_type;
        typename T::container_type;
    } &&
    std::is_same_v<std::remove_const_t<T>,
                   std::experimental::mdarray<
                       typename T::element_type, typename T::extents_type,
                       typename T::layout_type, typename T::container_type>>;

template <typename T>
concept md_c = mdspan_c<T> || mdarray_c<T>;

namespace detail {

template <extents_c extent_t>
[[nodiscard]] inline constexpr size_t static_mdarray_size() noexcept {
    if constexpr (extent_t::rank() == 0) {
        return 0;

    } else {
        return []<size_t... Is>(std::index_sequence<Is...>) {
            return (extent_t::static_extent(Is) * ...);
        }(std::make_index_sequence<extent_t::rank()>{});
    }
}

} // namespace detail

template <typename T, extents_c extent_t>
using mdarray = std::conditional_t<
    extent_t::rank_dynamic() == 0,
    std::experimental::mdarray<
        T, extent_t, layout_right,
        std::array<T, detail::static_mdarray_size<extent_t>()>>,
    std::experimental::mdarray<T, extent_t, layout_right, std::vector<T>>>;

template <size_t start, size_t end>
using slice = std::experimental::strided_slice<
    std::integral_constant<size_t, start>,
    std::integral_constant<size_t, end - start>,
    std::integral_constant<size_t, 1>>;

enum class MPMode : uint8_t { NONE, CPUMP };

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

template <typename T> using element_type_t = typename element_type<T>::type;

} // namespace ctmd
