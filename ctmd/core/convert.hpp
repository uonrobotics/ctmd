#pragma once

#include "./type.hpp"

namespace ctmd {
namespace core {

template <typename InType>
[[nodiscard]] inline constexpr auto to_mdspan(InType &&In) noexcept {
    using BaseType = std::remove_reference_t<InType>;

    if constexpr (mdspan_c<BaseType>) {
        return std::forward<InType>(In);

    } else if constexpr (requires { In.to_mdspan(); }) {
        return In.to_mdspan();

    } else {
        auto exts = extents<size_t>{};
        return mdspan<BaseType, decltype(exts)>{&In, exts};
    }
}

template <mdspan_c in_t>
[[nodiscard]] inline constexpr auto to_const(const in_t &in) noexcept {
    if constexpr (std::is_const_v<typename in_t::element_type>) {
        return std::forward<const in_t>(in);

    } else {
        using element_t = const typename in_t::value_type;

        return mdspan<element_t, typename in_t::extents_type,
                      typename in_t::layout_type, default_accessor<element_t>>(
            in.data_handle(), in.mapping(), {});
    }
}

template <typename InType>
[[nodiscard]] inline constexpr auto to_const_mdspan(InType &&In) noexcept {
    return to_const(to_mdspan(std::forward<InType>(In)));
}

template <mdspan_c in_t>
[[nodiscard]] inline constexpr bool is_always_reshapable() noexcept {
    return in_t::is_always_unique() && in_t::is_always_exhaustive() &&
           in_t::is_always_strided();
}

template <mdspan_c in_t>
[[nodiscard]] inline constexpr bool is_reshapable(const in_t &in) noexcept {
    return in.is_unique() && in.is_exhaustive() && in.is_strided();
}

template <typename InType, extents_c extents_t>
[[nodiscard]] inline constexpr auto
reshape(InType &&In, const extents_t &new_extents = extents_t{}) noexcept {
    const auto in = to_mdspan(std::forward<InType>(In));
    using in_t = decltype(in);

    assert(is_reshapable(in));
    assert(ctmd::size(in.extents()) == ctmd::size(new_extents));

    return mdspan<typename in_t::element_type, extents_t>{in.data_handle(),
                                                          new_extents};
}

template <typename T>
using to_mdspan_t = decltype(to_mdspan(std::declval<T>()));

template <typename T>
using to_mdcontainer_t =
    std::conditional_t<to_mdspan_t<T>::rank() == 0,
                       typename to_mdspan_t<T>::value_type,
                       mdarray<typename to_mdspan_t<T>::value_type,
                               typename to_mdspan_t<T>::extents_type>>;

} // namespace core
} // namespace ctmd
