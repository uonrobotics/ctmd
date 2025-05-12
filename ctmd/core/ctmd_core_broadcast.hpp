#pragma once

#include "ctmd_core_submdspan.hpp"
#include "ctmd_core_type.hpp"

namespace ctmd {
namespace core {

template <size_t Rank, extents_c in_t>
[[nodiscard]] inline constexpr auto
slice_from_start(const in_t &in = in_t{}) noexcept {
    return [&in]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<typename in_t::index_type, in_t::static_extent(Is)...>{
            in.extent(Is)...};
    }(std::make_index_sequence<Rank>{});
}

template <size_t Rank, extents_c in_t>
[[nodiscard]] inline constexpr auto
slice_from_last(const in_t &in = in_t{}) noexcept {
    return [&in]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<typename in_t::index_type,
                       in_t::static_extent(in_t::rank() - Rank + Is)...>{
            in.extent(in_t::rank() - Rank + Is)...};
    }(std::make_index_sequence<Rank>{});
}

template <extents_c in1_t, extents_c in2_t, extents_c... ins_t>
[[nodiscard]] inline constexpr auto concatenate(const in1_t &in1 = in1_t{},
                                                const in2_t &in2 = in2_t{},
                                                const ins_t &...ins) noexcept {
    using index_t = std::common_type_t<typename in1_t::index_type,
                                       typename in2_t::index_type>;

    const auto cexts = [&in1, &in2]<size_t... Is>(std::index_sequence<Is...>) {
        return [&in1, &in2]<size_t... Js>(std::index_sequence<Js...>) {
            return extents<index_t, in1_t::static_extent(Is)...,
                           in2_t::static_extent(Js)...>{
                static_cast<index_t>(in1.extent(Is))...,
                static_cast<index_t>(in2.extent(Js))...};
        }(std::make_index_sequence<in2_t::rank()>());
    }(std::make_index_sequence<in1_t::rank()>());

    if constexpr (sizeof...(ins_t) == 0) {
        return cexts;

    } else {
        return concatenate(cexts, ins...);
    }
}

template <extents_c in1_t, extents_c in2_t, extents_c... ins_t>
[[nodiscard]] inline constexpr auto broadcast(const in1_t &in1 = in1_t{},
                                              const in2_t &in2 = in2_t{},
                                              const ins_t &...ins) noexcept {
    using index_t = std::common_type_t<typename in1_t::index_type,
                                       typename in2_t::index_type>;
    constexpr size_t brank = std::max(in1_t::rank(), in2_t::rank());

    const auto bexts = [&brank, &in1,
                        &in2]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<index_t, [&brank]() {
            constexpr size_t x1 =
                (Is < brank - in1_t::rank()
                     ? 1
                     : in1_t::static_extent(Is - (brank - in1_t::rank())));
            constexpr size_t x2 =
                (Is < brank - in2_t::rank()
                     ? 1
                     : in2_t::static_extent(Is - (brank - in2_t::rank())));

            static_assert(x1 == x2 || x1 == 1 || x1 == dyn || x2 == 1 ||
                              x2 == dyn,
                          "Incompatible extents for broadcasting.");

            return std::max(x1, x2);
        }()...>{[&brank, &in1, &in2]() {
            const size_t x1 = (Is < brank - in1_t::rank()
                                   ? 1
                                   : in1.extent(Is - (brank - in1_t::rank())));
            const size_t x2 = (Is < brank - in2_t::rank()
                                   ? 1
                                   : in2.extent(Is - (brank - in2_t::rank())));

            assert(x1 == x2 || x1 == 1 || x2 == 1);

            return std::max(x1, x2);
        }()...};
    }(std::make_index_sequence<brank>{});

    if constexpr (sizeof...(ins_t) == 0) {
        return bexts;

    } else {
        return broadcast(bexts, ins...);
    }
}

template <extents_c... ins_t>
[[nodiscard]] inline constexpr auto
broadcast(const std::tuple<ins_t...> &ins) noexcept {
    constexpr size_t ins_num =
        std::tuple_size_v<std::remove_reference_t<decltype(ins)>>;

    static_assert(ins_num > 0, "broadcast requires at least one input.");

    if constexpr (ins_num == 1) {
        return std::get<0>(ins);

    } else {
        return std::apply(
            [&](auto &&...ins_refs) {
                return broadcast(std::forward<decltype(ins_refs)>(ins_refs)...);
            },
            ins);
    }
}

template <mdspan_c in_t, extents_c extents_t>
[[nodiscard]] inline constexpr auto
broadcast_to(const in_t &in = in_t{},
             const extents_t &new_extents = extents_t{}) noexcept {
    if constexpr (extents_t::rank() == 0) {
        return mdspan<typename in_t::element_type, extents_t, layout_right,
                      typename in_t::accessor_type>{in.data_handle()};

    } else if constexpr (in_t::rank() == extents_t::rank() &&
                         in_t::rank_dynamic() == 0 &&
                         extents_t::rank_dynamic() == 0 &&
                         []<size_t... Is>(std::index_sequence<Is...>) {
                             return ((in_t::static_extent(Is) ==
                                      extents_t::static_extent(Is)) &&
                                     ...);
                         }(std::make_index_sequence<in_t::rank()>{})) {
        return core::to_mdspan(in);

    } else if constexpr (in_t::rank() == 0) {
        auto new_strides =
            std::array<typename extents_t::size_type, extents_t::rank()>{};

        for (size_t i = 0; i < extents_t::rank(); i++) {
            new_strides[i] = 0;
        }

        return mdspan<typename in_t::element_type, extents_t, layout_stride,
                      typename in_t::accessor_type>{
            in.data_handle(), layout_stride::mapping{new_extents, new_strides}};

    } else {
        auto new_strides =
            std::array<typename extents_t::size_type, extents_t::rank()>{};

        for (size_t i = 0; i < extents_t::rank(); i++) {
            if (i < extents_t::rank() - in_t::rank()) {
                new_strides[i] = 0;

            } else {
                const size_t j = i - (extents_t::rank() - in_t::rank());

                if (in.extent(j) == new_extents.extent(i)) {
                    new_strides[i] = static_cast<typename extents_t::size_type>(
                        in.stride(j));

                } else if (in.extent(j) == 1) {
                    new_strides[i] = 0;

                } else {
                    assert(false);
                }
            }
        }

        return mdspan<typename in_t::element_type, extents_t, layout_stride,
                      typename in_t::accessor_type>{
            in.data_handle(), layout_stride::mapping{new_extents, new_strides}};
    }
}

namespace detail {

template <typename Func, mdspan_c... ins_t, typename... args_t, size_t... Is,
          size_t... Js>
inline constexpr void batch_call(const Func &&func,
                                 const std::tuple<ins_t...> &ins,
                                 const std::tuple<args_t...> &args,
                                 const std::index_sequence<Is...> &,
                                 const std::index_sequence<Js...> &) {
    func(std::get<Is>(ins)..., std::get<Js>(args)...);
}

template <mdspan_c... ins_t>
[[nodiscard]] inline constexpr auto
make_submdspan_tuple(const std::tuple<ins_t...> &ins,
                     const size_t idx) noexcept {
    return std::apply(
        [&](auto &&...refs) {
            return std::tuple{submdspan_from_start(
                std::forward<decltype(refs)>(refs), idx)...};
        },
        ins);
}

template <size_t BatchRank, typename Func, mdspan_c... ins_t,
          typename... args_t>
inline constexpr void
batch_impl(const Func &&func, const std::tuple<ins_t...> &ins,
           const std::tuple<args_t...> &args = std::tuple{}) noexcept {
    if constexpr (BatchRank == 0) {
        batch_call(std::move(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
        using index_type =
            typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl<BatchRank - 1>(std::move(func),
                                      make_submdspan_tuple(ins, i), args);
        }
    }
}

#ifdef _OPENMP

template <size_t BatchRank, typename Func, mdspan_c... ins_t,
          typename... args_t>
inline constexpr void
batch_impl_cpump(Func &&func, const std::tuple<ins_t...> &ins,
                 const std::tuple<args_t...> &args = std::tuple{}) noexcept {
    if constexpr (BatchRank == 0) {
        batch_call(std::move(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
        using index_type =
            typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

#pragma omp parallel for
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl<BatchRank - 1>(std::move(func),
                                      make_submdspan_tuple(ins, i), args);
        }
    }
}

#if false // TODO: fix this

template <size_t BatchRank, typename Func, mdspan_c... ins_t, typename... args_t>
inline constexpr void
batch_impl_gpump(Func &&func, const std::tuple<ins_t...> &ins,
                 const std::tuple<args_t...> &args = std::tuple{}) noexcept {
    if constexpr (BatchRank == 0) {
        batch_call(std::move(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
        using index_type =
            typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

#pragma omp target teams distribute parallel for
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl<BatchRank - 1>(std::move(func),
                                      make_submdspan_tuple(ins, i), args);
        }
    }
}

#endif

#endif

template <typename T, extents_c exts_t>
[[nodiscard]] inline constexpr auto create_out(const exts_t &exts) noexcept {
    if constexpr (exts_t::rank() == 0) {
        return T{};

    } else {
        return ctmd::mdarray<T, exts_t>{exts};
    }
}

} // namespace detail

template <typename Func, mdspan_c... ins_t, extents_c... uinexts_t,
          typename... args_t>
    requires(sizeof...(ins_t) == sizeof...(uinexts_t))
inline constexpr void batch(Func &&func, std::tuple<ins_t...> &&ins,
                            std::tuple<uinexts_t...> &&uinexts,
                            std::tuple<args_t...> &&args,
                            const MPMode mpmode) noexcept {
    const auto bexts = [&ins]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(std::make_tuple(
            slice_from_start<
                std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()>(
                std::get<Is>(ins).extents())...));
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    const auto bins = [&ins, &uinexts,
                       &bexts]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(broadcast_to(
            std::get<Is>(ins), concatenate(bexts, std::get<Is>(uinexts)))...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    if (mpmode == MPMode::NONE || std::is_constant_evaluated()) [[likely]] {
        detail::batch_impl<decltype(bexts)::rank()>(std::move(func), bins,
                                                    std::move(args));
    } else if (mpmode == MPMode::CPUMP) {
#ifdef _OPENMP
        detail::batch_impl_cpump<decltype(bexts)::rank()>(std::move(func), bins,
                                                          std::move(args));
#else
        assert(false);
#endif
    } else {
        assert(false);
    }
}

template <typename Func, mdspan_c... ins_t, extents_c... uinexts_t,
          typename... args_t>
    requires(sizeof...(ins_t) == sizeof...(uinexts_t) - 1)
inline constexpr auto batch(Func &&func, std::tuple<ins_t...> &&ins,
                            std::tuple<uinexts_t...> &&uinexts,
                            std::tuple<args_t...> &&args,
                            const MPMode mpmode) noexcept {
    const auto bexts = [&ins]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(std::make_tuple(
            slice_from_start<
                std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()>(
                std::get<Is>(ins).extents())...));
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    auto out_exts =
        core::concatenate(bexts, std::get<sizeof...(uinexts_t) - 1>(uinexts));
    using element_t = std::common_type_t<element_type_t<ins_t>...>;

    auto out = detail::create_out<element_t>(out_exts);

    const auto bins = [&ins, &uinexts, &bexts,
                       &out]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(
            broadcast_to(std::get<Is>(ins),
                         concatenate(bexts, std::get<Is>(uinexts)))...,
            to_mdspan(out));
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    if (mpmode == MPMode::NONE || std::is_constant_evaluated()) [[likely]] {
        detail::batch_impl<decltype(bexts)::rank()>(std::move(func), bins,
                                                    std::move(args));
    } else if (mpmode == MPMode::CPUMP) {
#ifdef _OPENMP
        detail::batch_impl_cpump<decltype(bexts)::rank()>(std::move(func), bins,
                                                          std::move(args));
#else
        assert(false);
#endif
    } else {
        assert(false);
    }

    return out;
}

} // namespace core
} // namespace ctmd
