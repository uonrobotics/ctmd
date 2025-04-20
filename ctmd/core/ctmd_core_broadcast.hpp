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

            assert(("Incompatible extents for broadcasting.",
                    x1 == x2 || x1 == 1 || x2 == 1));

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
broadcast(std::tuple<ins_t...> &&ins) noexcept {
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

template <md_c in_t, extents_c extents_t>
[[nodiscard]] inline constexpr auto
broadcast_to(in_t &in = in_t{},
             const extents_t &new_extents = extents_t{}) noexcept {
    if constexpr (in_t::rank() == extents_t::rank() &&
                  in_t::rank_dynamic() == 0 && extents_t::rank_dynamic() == 0 &&
                  []<size_t... Is>(std::index_sequence<Is...>) {
                      return ((in_t::static_extent(Is) ==
                               extents_t::static_extent(Is)) &&
                              ...);
                  }(std::make_index_sequence<in_t::rank()>{})) {
        return core::to_mdspan(in);

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

template <typename Func, mdspan_c... ins_t, mdspan_c... outs_t,
          typename... Args, size_t... Is, size_t... Js, size_t... Ks>
inline constexpr void
batch_call(Func &&func, std::tuple<ins_t...> &&ins,
           std::tuple<outs_t...> &&outs, std::tuple<Args...> &&args,
           std::index_sequence<Is...>, std::index_sequence<Js...>,
           std::index_sequence<Ks...>) {
    func(std::get<Is>(ins)..., std::get<Js>(outs)..., std::get<Ks>(args)...);
}

template <size_t BatchRank, typename Func, mdspan_c... uins_t,
          mdspan_c... uouts_t, mdspan_c... ins_t, mdspan_c... outs_t,
          typename... Args>
    requires(sizeof...(uins_t) == sizeof...(ins_t) &&
             sizeof...(uouts_t) == sizeof...(outs_t))
inline constexpr void
batch_impl(Func &&func, std::tuple<uins_t...> &&uins,
           std::tuple<uouts_t...> &&uouts, std::tuple<ins_t...> &&ins,
           std::tuple<outs_t...> &&outs,
           std::tuple<Args...> &&args = std::tuple<>{}) noexcept {
    using index_type =
        typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

    auto make_subtuple = [&](auto &&tuple, index_type i) {
        return std::apply(
            [&](auto &&...refs) {
                return std::make_tuple(submdspan_from_start(
                    std::forward<decltype(refs)>(refs), i)...);
            },
            std::forward<decltype(tuple)>(tuple));
    };

    if constexpr (BatchRank == 0) {
        static_assert(std::is_invocable_v<Func, std::remove_cvref_t<ins_t>...,
                                          std::remove_cvref_t<outs_t>...,
                                          std::remove_cvref_t<Args>...>,
                      "Func must be invocable with: ins..., outs..., args...");

        batch_call(std::move(func), std::move(ins), std::move(outs),
                   std::move(args), std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<outs_t...>{},
                   std::index_sequence_for<Args...>{});

    } else {
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            auto subins = make_subtuple(ins, i);
            auto subouts = make_subtuple(outs, i);

            batch_impl<BatchRank - 1>(std::move(func), std::move(uins),
                                      std::move(uouts), std::move(subins),
                                      std::move(subouts), std::move(args));
        }
    }
}

template <size_t BatchRank, typename Func, mdspan_c... uins_t,
          mdspan_c... uouts_t, mdspan_c... ins_t, mdspan_c... outs_t,
          typename... Args>
    requires(sizeof...(uins_t) == sizeof...(ins_t) &&
             sizeof...(uouts_t) == sizeof...(outs_t))
inline void
batch_impl_omp(Func &&func, std::tuple<uins_t...> &&uins,
               std::tuple<uouts_t...> &&uouts, std::tuple<ins_t...> &&ins,
               std::tuple<outs_t...> &&outs,
               std::tuple<Args...> &&args = std::tuple<>{}) noexcept {
    using index_type =
        typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

    auto make_subtuple = [&](auto &&tuple, index_type i) {
        return std::apply(
            [&](auto &&...refs) {
                return std::make_tuple(submdspan_from_start(
                    std::forward<decltype(refs)>(refs), i)...);
            },
            std::forward<decltype(tuple)>(tuple));
    };

    if constexpr (BatchRank == 0) {
        static_assert(std::is_invocable_v<Func, std::remove_cvref_t<ins_t>...,
                                          std::remove_cvref_t<outs_t>...,
                                          std::remove_cvref_t<Args>...>,
                      "Func must be invocable with: ins..., outs..., args...");

        batch_call(std::move(func), std::move(ins), std::move(outs),
                   std::move(args), std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<outs_t...>{},
                   std::index_sequence_for<Args...>{});

    } else {
#pragma omp parallel for
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            auto subins = make_subtuple(ins, i);
            auto subouts = make_subtuple(outs, i);

            batch_impl<BatchRank - 1>(std::move(func), std::move(uins),
                                      std::move(uouts), std::move(subins),
                                      std::move(subouts), std::move(args));
        }
    }
}

template <size_t BatchRank, typename Func, mdspan_c... uins_t,
          mdspan_c... uouts_t, mdspan_c... ins_t, mdspan_c... outs_t,
          typename... Args>
    requires(sizeof...(uins_t) == sizeof...(ins_t) &&
             sizeof...(uouts_t) == sizeof...(outs_t))
inline void batch_impl_gpu(Func &&func, const std::tuple<uins_t...> &uins,
                           const std::tuple<uouts_t...> &uouts,
                           const std::tuple<ins_t...> &ins,
                           const std::tuple<outs_t...> &outs,
                           const std::tuple<Args...> &args) {
    using index_type =
        typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

    auto make_subtuple = [](auto &&tuple, index_type i) {
        return std::apply(
            [&](auto &&...refs) {
                return std::make_tuple(submdspan_from_start(refs, i)...);
            },
            tuple);
    };

    if constexpr (BatchRank == 0) {
        static_assert(std::is_invocable_v<Func, ins_t..., outs_t..., Args...>,
                      "Function signature mismatch in batch_impl_gpu.");

        std::apply(
            [&](const auto &...in_refs) {
                std::apply(
                    [&](const auto &...out_refs) {
                        std::apply(
                            [&](const auto &...arg_vals) {
                                func(in_refs..., out_refs..., arg_vals...);
                            },
                            args);
                    },
                    outs);
            },
            ins);

    } else {
        const index_type extent0 = std::get<0>(ins).extent(0);

#pragma acc parallel loop
        for (index_type i = 0; i < extent0; ++i) {
            const auto subins = make_subtuple(ins, i);
            const auto subouts = make_subtuple(outs, i);

            batch_impl_gpu<BatchRank - 1>(func, uins, uouts, subins, subouts,
                                          args);
        }
    }
}

} // namespace detail

template <typename Func, mdspan_c... uins_t, mdspan_c... uouts_t,
          mdspan_c... ins_t, mdspan_c... outs_t, typename... Args>
    requires(sizeof...(uins_t) == sizeof...(ins_t) &&
             sizeof...(uouts_t) == sizeof...(outs_t))
inline constexpr void
batch(Func &&func, std::tuple<uins_t...> &&uins, std::tuple<uouts_t...> &&uouts,
      std::tuple<ins_t...> &&ins, std::tuple<outs_t...> &&outs,
      std::tuple<Args...> &&args = std::tuple<>{},
      bool multi_process = false) noexcept {
    const auto bexts = [&ins, &outs]<size_t... Is>(std::index_sequence<Is...>) {
        return [&ins, &outs]<size_t... Js>(std::index_sequence<Js...>) {
            return broadcast(std::make_tuple(
                slice_from_start<
                    std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                    std::tuple_element_t<Is, std::tuple<uins_t...>>::rank()>(
                    std::get<Is>(ins).extents())...,
                slice_from_start<
                    std::tuple_element_t<Js, std::tuple<outs_t...>>::rank() -
                    std::tuple_element_t<Js, std::tuple<uouts_t...>>::rank()>(
                    std::get<Js>(outs).extents())...));
        }(std::make_index_sequence<sizeof...(outs_t)>{});
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    auto bins = [&ins, &bexts]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(broadcast_to(
            std::get<Is>(ins),
            concatenate(
                bexts,
                slice_from_last<
                    std::tuple_element_t<Is, std::tuple<uins_t...>>::rank()>(
                    std::get<Is>(ins).extents())))...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    auto bouts = [&outs, &bexts]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(broadcast_to(
            std::get<Is>(outs),
            concatenate(
                bexts,
                slice_from_last<
                    std::tuple_element_t<Is, std::tuple<uouts_t...>>::rank()>(
                    std::get<Is>(outs).extents())))...);
    }(std::make_index_sequence<sizeof...(outs_t)>{});

    if (std::is_constant_evaluated() || !multi_process) {
        detail::batch_impl<decltype(bexts)::rank()>(
            std::move(func), std::move(uins), std::move(uouts), std::move(bins),
            std::move(bouts), std::move(args));

    } else {
        // TODO: batch_impl_omp make slow when element_type is uint8_t, int8_t,
        // even function is not called. check it.

#if false
        detail::batch_impl_omp<decltype(bexts)::rank()>(
            std::move(func), std::move(uins), std::move(uouts), std::move(bins),
            std::move(bouts), std::move(args));

#else
        detail::batch_impl_gpu<decltype(bexts)::rank()>(
            std::move(func), std::move(uins), std::move(uouts), std::move(bins),
            std::move(bouts), std::move(args));

#endif
    }
}

template <typename Func, mdspan_c... uins_t, mdspan_c... uouts_t,
          mdspan_c... ins_t, typename... Args>
    requires(sizeof...(uins_t) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, std::tuple<uins_t...> &&uins,
          std::tuple<uouts_t...> &&uouts, std::tuple<ins_t...> &&ins,
          std::tuple<Args...> &&args = std::tuple<>{},
          bool multi_process = false) noexcept {
    const auto bexts = [&ins]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(std::make_tuple(
            slice_from_start<
                std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<Is, std::tuple<uins_t...>>::rank()>(
                std::get<Is>(ins).extents())...));
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    auto outs_data = [&bexts,
                      &uouts]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple([&bexts, &uouts]() {
            const auto &uout = std::get<Is>(uouts);
            using uout_t = std::remove_reference_t<decltype(uout)>;
            const auto exts = concatenate(bexts, uout.extents());
            return mdarray<typename uout_t::element_type,
                           std::remove_cvref_t<decltype(exts)>>{exts};
        }()...);
    }(std::make_index_sequence<sizeof...(uouts_t)>{});

    auto outs = [&outs_data]<size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(core::to_mdspan(std::get<Is>(outs_data))...);
    }(std::make_index_sequence<sizeof...(uouts_t)>{});

    batch(std::move(func), std::move(uins), std::move(uouts), std::move(ins),
          std::move(outs), std::move(args), multi_process);

    return outs_data;
}

} // namespace core
} // namespace ctmd
