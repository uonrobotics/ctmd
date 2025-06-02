#pragma once

#include "ctmd_core_type.hpp"

namespace ctmd {
namespace core {

template <typename InType>
[[nodiscard]] inline constexpr auto to_mdspan(InType &&In) noexcept {
    using BaseType = std::remove_reference_t<InType>;

    if constexpr (mdspan_c<BaseType>) {
        return In;

    } else if constexpr (mdarray_c<BaseType>) {
        return In.to_mdspan();

    } else if constexpr (arithmetic_c<BaseType>) {
        auto exts = extents<size_t>{};
        return mdspan<BaseType, decltype(exts)>{&In, exts};

    } else {
        static_assert(std::false_type::value, "Invalid type");
    }
}

template <mdspan_c in_t>
[[nodiscard]] inline constexpr auto to_const(const in_t &in) noexcept {
    using const_element_t = const typename in_t::element_type;
    return mdspan<const_element_t, typename in_t::extents_type,
                  typename in_t::layout_type,
                  default_accessor<const_element_t>>(in.data_handle(),
                                                     in.mapping(), {});
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

} // namespace core
} // namespace ctmd
