#pragma once

#include "core/ctmd_core.hpp"

namespace ctmd {

template <int64_t Axis, typename in_t>
[[nodiscard]] inline constexpr auto expand_dims(in_t &in) noexcept {
    auto rin = core::to_mdspan(in);
    using rin_t = decltype(rin);

    constexpr size_t rank = rin_t::rank();

    constexpr size_t axis =
        static_cast<size_t>(((Axis % (rank + 1)) + (rank + 1)) % (rank + 1));

    const auto new_extents = [&rin]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<
            typename rin_t::index_type,
            (Is < axis ? rin_t::static_extent(Is)
                       : (Is == axis ? 1 : rin_t::static_extent(Is - 1)))...>{
            (Is < axis ? rin.extent(Is)
                       : (Is == axis ? 1 : rin.extent(Is - 1)))...};
    }(std::make_index_sequence<rank + 1>{});

    const auto new_strides = [&rin]<size_t... Is>(std::index_sequence<Is...>) {
        return std::array<typename rin_t::size_type, rank + 1>{
            (Is < axis ? rin.stride(Is)
                       : (Is == axis ? 1 : rin.stride(Is - 1)))...};
    }(std::make_index_sequence<rank + 1>{});

    return mdspan<typename rin_t::element_type,
                  std::remove_const_t<decltype(new_extents)>, layout_stride,
                  typename rin_t::accessor_type>{
        rin.data_handle(), layout_stride::mapping{new_extents, new_strides}};
}

} // namespace ctmd
