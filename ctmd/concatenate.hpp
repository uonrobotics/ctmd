#pragma once

#include "copy.hpp"
#include "empty.hpp"
#include "expand_dims.hpp"

namespace ctmd {
namespace detail {

template <typename in1_t, typename in2_t, size_t Is, size_t axis>
constexpr size_t concatenate_static_extent() {
    constexpr size_t e1 = in1_t::static_extent(Is);
    constexpr size_t e2 = in2_t::static_extent(Is);

    if constexpr (Is == axis) {
        return (e1 != dyn && e2 != dyn) ? (e1 + e2) : dyn;

    } else {
        static_assert(e1 == e2 || e1 == dyn || e2 == dyn,
                      "Incompatible extents for concatenate.");
        return (e1 != dyn && e2 != dyn) ? e1 : dyn;
    }
}

template <int64_t Axis, extents_c in1_t, extents_c in2_t, extents_c... ins_t>
    requires(in1_t::rank() == in2_t::rank())
[[nodiscard]] inline constexpr auto
concatenate_extents(const in1_t &in1 = in1_t{}, const in2_t &in2 = in2_t{},
                    const ins_t &...ins) noexcept {
    constexpr size_t rank = in1_t::rank();
    constexpr size_t axis = static_cast<size_t>(
        ((Axis % static_cast<int64_t>(rank)) + (rank)) % rank);

    using index_t = std::common_type_t<typename in1_t::index_type,
                                       typename in2_t::index_type>;

    const auto exts = [&in1, &in2]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<index_t,
                       concatenate_static_extent<in1_t, in2_t, Is, axis>()...>{
            [&in1, &in2]() {
                const size_t e1 = in1.extent(Is);
                const size_t e2 = in2.extent(Is);

                if constexpr (Is == axis) {
                    return e1 + e2;

                } else {
                    assert(e1 == e2);
                    return e1;
                }
            }()...};
    }(std::make_index_sequence<rank>{});

    if constexpr (sizeof...(ins_t) != 0) {
        return concatenate_extents<axis>(exts, ins...);

    } else {
        return exts;
    }
}

template <int64_t Axis, extents_c... ins_t>
[[nodiscard]] inline constexpr auto
concatenate_extents(const std::tuple<ins_t...> &ins) noexcept {
    constexpr size_t ins_num =
        std::tuple_size_v<std::remove_reference_t<decltype(ins)>>;

    static_assert(ins_num != 0, "concatenate requires at least one input.");

    if constexpr (ins_num == 1) {
        return std::get<0>(ins);

    } else {
        return std::apply(
            [&](auto &&...elems) {
                return concatenate_extents<Axis>(
                    std::forward<decltype(elems)>(elems)...);
            },
            ins);
    }
}

} // namespace detail

template <int64_t Axis, typename... InsTypes>
[[nodiscard]] inline constexpr auto
concatenate(std::tuple<InsTypes...> &&Ins) noexcept {
    constexpr size_t num_ins = sizeof...(InsTypes);

    // generate input mdspans
    const auto ins = [&Ins]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple([&Ins]() {
            auto mds = core::to_const_mdspan(std::get<Is>(Ins));
            if constexpr (mds.rank() == 0) {
                // if the input is a scalar, expand it to a 1D mdspan.
                return ctmd::expand_dims<-1>(mds);

            } else {
                return mds;
            }
        }()...);
    }(std::make_index_sequence<num_ins>{});

    using ins_t = decltype(ins);

    // generate out extents
    constexpr size_t rank = std::tuple_element_t<0, ins_t>::rank();
    constexpr size_t axis = static_cast<size_t>(
        ((Axis % static_cast<int64_t>(rank)) + (rank)) % rank);

    const auto out_extents = [&ins]<size_t... Is>(std::index_sequence<Is...>) {
        return detail::concatenate_extents<axis>(
            std::make_tuple(std::get<Is>(ins).extents()...));
    }(std::make_index_sequence<num_ins>{});

    // generate out
    using T = decltype([]<size_t... Is>(std::index_sequence<Is...>) {
        return std::common_type_t<
            value_type_t<std::tuple_element_t<Is, ins_t>>...>{};
    }(std::make_index_sequence<num_ins>{}));
    auto out = ctmd::empty<T>(out_extents);

    // concatenate
    [&ins, &out]<size_t... Is>(std::index_sequence<Is...>) {
        (([&] {
             const size_t offset =
                 [&]<size_t... Js>(std::index_sequence<Js...>) {
                     return (0 + ... + std::get<Js>(ins).extent(axis));
                 }(std::make_index_sequence<Is>{});
             const size_t extent = std::get<Is>(ins).extent(axis);
             constexpr size_t stride = 1;

             ctmd::copy(std::get<Is>(ins), [&]<size_t... Js>(
                                               std::index_sequence<Js...>) {
                 return core::submdspan_from_left(
                     core::to_mdspan(out), ((void)Js, ctmd::full_extent)...,
                     ctmd::strided_slice{offset, extent, stride});
             }(std::make_index_sequence<axis>{}));
         })(),
         ...);
    }(std::make_index_sequence<num_ins>{});

    return out;
}

} // namespace ctmd
