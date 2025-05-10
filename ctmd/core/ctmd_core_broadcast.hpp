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
inline constexpr void batch_call(Func &&func, const std::tuple<ins_t...> &ins,
                                 const std::tuple<args_t...> &args,
                                 const std::index_sequence<Is...> &,
                                 const std::index_sequence<Js...> &) {
    func(std::get<Is>(ins)..., std::get<Js>(args)...);
}

template <size_t BatchRank, typename Func, typename... ins_t,
          typename... args_t>
inline constexpr void
batch_impl(Func &&func, const std::tuple<ins_t...> &ins,
           const std::tuple<args_t...> &args = std::tuple{}) noexcept {
    using index_type =
        typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

    const auto make_subtuple = [&](auto &&tuple, index_type i) {
        return std::apply(
            [&](auto &&...refs) {
                return std::tuple{submdspan_from_start(
                    std::forward<decltype(refs)>(refs), i)...};
            },
            std::forward<decltype(tuple)>(tuple));
    };

    if constexpr (BatchRank == 0) {
        batch_call(std::move(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl<BatchRank - 1>(std::move(func), make_subtuple(ins, i),
                                      args);
        }
    }
}

#ifdef _OPENMP

template <size_t BatchRank, typename Func, mdspan_c... ins_t,
          typename... args_t>
inline constexpr void
batch_impl_cpump(Func &&func, const std::tuple<ins_t...> &ins,
                 const std::tuple<args_t...> &args = std::tuple{}) noexcept {
    using index_type =
        typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

    const auto make_subtuple = [&](auto &&tuple, index_type i) {
        return std::apply(
            [&](auto &&...refs) {
                return std::tuple{submdspan_from_start(
                    std::forward<decltype(refs)>(refs), i)...};
            },
            std::forward<decltype(tuple)>(tuple));
    };

    if constexpr (BatchRank == 0) {
        batch_call(std::move(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
#pragma omp parallel for
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl<BatchRank - 1>(std::move(func), make_subtuple(ins, i),
                                      args);
        }
    }
}

#if false // TODO: fix this

template <size_t BatchRank, typename Func, mdspan_c... ins_t, typename... args_t>
inline constexpr void
batch_impl_gpump(Func &&func, const std::tuple<ins_t...> &ins,
                 const std::tuple<args_t...> &args = std::tuple{}) noexcept {
    using index_type =
        typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

    const auto make_subtuple = [&](auto &&tuple, index_type i) {
        return std::apply(
            [&](auto &&...refs) {
                return std::tuple{submdspan_from_start(
                    std::forward<decltype(refs)>(refs), i)...};
            },
            std::forward<decltype(tuple)>(tuple));
    };

    if constexpr (BatchRank == 0) {
        batch_call(std::move(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
#pragma omp target teams distribute parallel for
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl<BatchRank - 1>(std::move(func), make_subtuple(ins, i),
                                      args);
        }
    }
}

#endif

#endif

} // namespace detail

template <size_t BatchRank, MPMode mpmode, typename Func, typename... ins_t,
          typename... args_t>
inline constexpr void
batch(Func &&func, const std::tuple<ins_t...> &ins,
      const std::tuple<args_t...> &args = std::tuple{}) noexcept {
    if constexpr (mpmode == MPMode::CPUMP) {
        if (!std::is_constant_evaluated()) [[likely]] {
#ifdef _OPENMP
            detail::batch_impl_cpump<BatchRank>(std::move(func), ins, args);
#else
            assert(false);
#endif
            return;
        }

#if false // TODO: fix this
        } else if constexpr (mpmode == MPMode::GPUMP) {
            if (!std::is_constant_evaluated()) [[likely]] {
#ifdef _OPENMP
                detail::batch_impl_gpump<BatchRank>(std::move(func), ins, args);
#else
                assert(false);
#endif
                return;
            }
#endif
    }

    detail::batch_impl<BatchRank>(std::move(func), ins, args);
}

} // namespace core
} // namespace ctmd
