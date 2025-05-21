#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <typename InType, extents_c extents_t>
[[nodiscard]] inline constexpr auto
reshape(InType &&In, const extents_t &new_extents = extents_t{}) noexcept {
    const auto in = core::to_mdspan(std::forward<InType>(In));
    using in_t = decltype(in);

    if constexpr (in_t::is_always_unique() && in_t::is_always_exhaustive() &&
                  in_t::is_always_strided() && in_t::rank_dynamic() == 0 &&
                  extents_t::rank_dynamic() == 0) {
        constexpr size_t in_size = core::size<typename in_t::extents_type>();
        constexpr size_t new_size = core::size<extents_t>();

        static_assert(in_size == new_size,
                      "The number of elements in the input and output "
                      "mdspan must be the same.");

        return mdspan<typename in_t::element_type, extents_t>{in.data_handle(),
                                                              new_extents};

    } else {
        assert(in.is_unique());
        assert(in.is_exhaustive());
        assert(in.is_strided());
        assert(core::size(in) == core::size(new_extents));

        return mdspan<typename in_t::element_type, extents_t>{in.data_handle(),
                                                              new_extents};
    }
}

} // namespace ctmd
