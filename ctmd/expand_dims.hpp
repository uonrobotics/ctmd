#pragma once

#include "reshape.hpp"

namespace ctmd {

template <int64_t Axis, typename InType>
[[nodiscard]] inline constexpr auto expand_dims(InType &&In) noexcept {
    const auto in = core::to_const_mdspan(std::forward<InType>(In));
    using in_t = decltype(in);

    constexpr size_t rank = in_t::rank();

    if constexpr (rank == 0) {
        auto new_extents = extents<typename in_t::index_type, 1>{1};
        return mdspan<typename in_t::element_type, decltype(new_extents)>{
            in.data_handle(), new_extents};

    } else {
        constexpr size_t axis = static_cast<size_t>(
            ((Axis % static_cast<int64_t>(rank + 1)) + (rank + 1)) %
            (rank + 1));

        const auto new_extents =
            [&in]<size_t... Is>(std::index_sequence<Is...>) {
                return extents<
                    typename in_t::index_type,
                    (Is < axis
                         ? in_t::static_extent(Is)
                         : (Is == axis ? 1 : in_t::static_extent(Is - 1)))...>{
                    (Is < axis ? in.extent(Is)
                               : (Is == axis ? 1 : in.extent(Is - 1)))...};
            }(std::make_index_sequence<rank + 1>{});

        return ctmd::reshape(std::forward<InType>(In), new_extents);
    }
}

} // namespace ctmd
