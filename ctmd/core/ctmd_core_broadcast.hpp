#pragma once

#include "ctmd_core_submdspan.hpp"
#include "ctmd_core_type.hpp"

namespace ctmd {
namespace core {

template <size_t SliceRank, extents_c in_t>
[[nodiscard]] inline constexpr auto
slice_from_start(const in_t &in = in_t{}) noexcept {
    static_assert(SliceRank <= in_t::rank(), "SliceRank exceeds rank.");

    return [&in]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<typename in_t::index_type, in_t::static_extent(Is)...>{
            in.extent(Is)...};
    }(std::make_index_sequence<SliceRank>{});
}

template <size_t SliceRank, extents_c in_t>
[[nodiscard]] inline constexpr auto
slice_from_last(const in_t &in = in_t{}) noexcept {
    static_assert(SliceRank <= in_t::rank(), "SliceRank exceeds rank.");

    return [&in]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<typename in_t::index_type,
                       in_t::static_extent(in_t::rank() - SliceRank + Is)...>{
            in.extent(in_t::rank() - SliceRank + Is)...};
    }(std::make_index_sequence<SliceRank>{});
}

template <extents_c in1_t, extents_c in2_t, extents_c... ins_t>
[[nodiscard]] inline constexpr auto concatenate(const in1_t &in1 = in1_t{},
                                                const in2_t &in2 = in2_t{},
                                                const ins_t &...ins) noexcept {
    using index_t = std::common_type_t<typename in1_t::index_type,
                                       typename in2_t::index_type>;

    const auto cexts =
        [&in1, &in2]<size_t... Is, size_t... Js>(std::index_sequence<Is...>,
                                                 std::index_sequence<Js...>) {
            return extents<index_t, in1_t::static_extent(Is)...,
                           in2_t::static_extent(Js)...>{
                static_cast<index_t>(in1.extent(Is))...,
                static_cast<index_t>(in2.extent(Js))...};
        }(std::make_index_sequence<in1_t::rank()>{},
          std::make_index_sequence<in2_t::rank()>{});

    if constexpr (sizeof...(ins_t) == 0) {
        return cexts;

    } else {
        return concatenate(cexts, ins...);
    }
}

namespace detail {

template <typename in1_t, typename in2_t, size_t Is, size_t brank>
constexpr size_t broadcast_static_extent() {
    constexpr size_t e1 =
        (Is < brank - in1_t::rank()
             ? 1
             : in1_t::static_extent(Is - (brank - in1_t::rank())));
    constexpr size_t e2 =
        (Is < brank - in2_t::rank()
             ? 1
             : in2_t::static_extent(Is - (brank - in2_t::rank())));
    static_assert(e1 == e2 || e1 == 1 || e1 == dyn || e2 == 1 || e2 == dyn,
                  "incompatible extents for broadcasting.");

    return std::max(e1, e2);
}

} // namespace detail

template <extents_c in1_t, extents_c in2_t, extents_c... ins_t>
[[nodiscard]] inline constexpr auto broadcast(const in1_t &in1 = in1_t{},
                                              const in2_t &in2 = in2_t{},
                                              const ins_t &...ins) noexcept {
    using index_t = std::common_type_t<typename in1_t::index_type,
                                       typename in2_t::index_type>;
    constexpr size_t brank = std::max(in1_t::rank(), in2_t::rank());

    const auto bexts = [&in1, &in2]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<index_t, detail::broadcast_static_extent<
                                    in1_t, in2_t, Is, brank>()...>{[&in1,
                                                                    &in2]() {
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
    if constexpr (in_t::rank() == extents_t::rank() &&
                  in_t::rank_dynamic() == 0 && extents_t::rank_dynamic() == 0 &&
                  []<size_t... Is>(std::index_sequence<Is...>) {
                      return ((in_t::static_extent(Is) ==
                               extents_t::static_extent(Is)) &&
                              ...);
                  }(std::make_index_sequence<in_t::rank()>{})) {
        return in;

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
inline constexpr void batch_call(Func &&func, const std::tuple<ins_t...> &ins,
                                 const std::tuple<args_t...> &args,
                                 const std::index_sequence<Is...> &,
                                 const std::index_sequence<Js...> &) {
    func(std::get<Is>(ins)..., std::get<Js>(args)...);
}

template <mdspan_c... ins_t, typename... slices_t>
[[nodiscard]] inline constexpr auto
make_submdspan_tuple(const std::tuple<ins_t...> &ins,
                     slices_t &&...slices) noexcept {
    return std::apply(
        [&](auto &&...elems) {
            return std::tuple{
                submdspan_from_start(std::forward<decltype(elems)>(elems),
                                     std::forward<slices_t>(slices)...)...};
        },
        ins);
}

template <size_t BatchRank, typename Func, mdspan_c... ins_t,
          typename... args_t>
inline constexpr void
batch_impl_none(Func &&func, const std::tuple<ins_t...> &ins,
                const std::tuple<args_t...> &args) noexcept {
    if constexpr (BatchRank == 0) {
        batch_call(std::forward<Func>(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
        using index_type =
            typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl_none<BatchRank - 1>(std::forward<Func>(func),
                                           make_submdspan_tuple(ins, i), args);
        }
    }
}

#ifdef _OPENMP

template <size_t BatchRank, typename Func, mdspan_c... ins_t,
          typename... args_t>
inline void batch_impl_cpump(Func &&func, const std::tuple<ins_t...> &ins,
                             const std::tuple<args_t...> &args) noexcept {
    if constexpr (BatchRank == 0) {
        batch_call(std::forward<Func>(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
        using index_type =
            typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

#pragma omp parallel for
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl_none<BatchRank - 1>(std::forward<Func>(func),
                                           make_submdspan_tuple(ins, i), args);
        }
    }
}

#if false // TODO: fix this

template <size_t BatchRank, typename Func, mdspan_c... ins_t,
          typename... args_t>
inline  void
batch_impl_gpump(Func &&func, const std::tuple<ins_t...> &ins,
                 const std::tuple<args_t...> &args) noexcept {
    if constexpr (BatchRank == 0) {
        batch_call(std::forward<Func>(func), ins, args,
                   std::index_sequence_for<ins_t...>{},
                   std::index_sequence_for<args_t...>{});

    } else {
        using index_type =
            typename std::tuple_element_t<0, std::tuple<ins_t...>>::index_type;

#pragma omp target teams distribute parallel for
        for (index_type i = 0; i < std::get<0>(ins).extent(0); i++) {
            batch_impl_none<BatchRank - 1>(std::forward<Func>(func),
                                      make_submdspan_tuple(ins, i), args);
        }
    }
}

#endif

#endif

template <size_t BatchRank, typename Func, mdspan_c... ins_t,
          typename... args_t>
inline constexpr void batch_impl(Func &&func, const std::tuple<ins_t...> &ins,
                                 const std::tuple<args_t...> &args,
                                 const MPMode mpmode) noexcept {
    if (!std::is_constant_evaluated()) [[likely]] {
        if (mpmode == MPMode::CPUMP) [[unlikely]] {
#ifdef _OPENMP
            detail::batch_impl_cpump<BatchRank>(std::forward<Func>(func), ins,
                                                args);
            return;
#else
            assert(false);
#endif
        }
    }

    detail::batch_impl_none<BatchRank>(std::forward<Func>(func), ins, args);
}

template <typename T, extents_c exts_t>
[[nodiscard]] inline constexpr auto create_out(const exts_t &exts) noexcept {
    if constexpr (exts_t::rank() == 0) {
        return T{};

    } else {
        return ctmd::mdarray<T, exts_t>{exts};
    }
}

} // namespace detail

template <mdspan_c... ins_t, extents_c... uinexts_t>
    requires(sizeof...(ins_t) == sizeof...(uinexts_t) - 1)
[[nodiscard]] inline constexpr auto
create_out(const std::tuple<ins_t...> &ins,
           const std::tuple<uinexts_t...> &uinexts) noexcept {
    using element_t = std::common_type_t<element_type_t<ins_t>...>;

    const auto bexts = [&ins]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(std::make_tuple(
            slice_from_start<
                std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()>(
                std::get<Is>(ins).extents())...));
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    return detail::create_out<element_t>(
        core::concatenate(bexts, std::get<sizeof...(ins_t)>(uinexts)));
}

template <mdspan_c... ins_t, extents_c... uinexts_t>
    requires(sizeof...(ins_t) < sizeof...(uinexts_t) - 1)
[[nodiscard]] inline constexpr auto
create_out(const std::tuple<ins_t...> &ins,
           const std::tuple<uinexts_t...> &uinexts) noexcept {
    using element_t = std::common_type_t<element_type_t<ins_t>...>;

    const auto bexts = [&ins]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(std::make_tuple(
            slice_from_start<
                std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()>(
                std::get<Is>(ins).extents())...));
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    return [&uinexts, &bexts]<size_t... Is>(std::index_sequence<Is...>) {
        return std::tuple{detail::create_out<element_t>(core::concatenate(
            bexts, std::get<sizeof...(ins_t) + Is>(uinexts)))...};
    }(std::make_index_sequence<sizeof...(uinexts_t) - sizeof...(ins_t)>{});
}

template <typename Func, mdspan_c... ins_t, extents_c... uinexts_t,
          typename... args_t>
    requires(sizeof...(ins_t) == sizeof...(uinexts_t))
inline constexpr void batch(Func &&func, const std::tuple<ins_t...> &ins,
                            const std::tuple<uinexts_t...> &uinexts,
                            const std::tuple<args_t...> &args,
                            const MPMode mpmode) noexcept {
    constexpr bool need_batch = []<size_t... Is>(std::index_sequence<Is...>) {
        return ((std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() !=
                 std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()) ||
                ...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    if constexpr (!need_batch) {
        // Pass directly to the function
        detail::batch_impl_none<0>(std::forward<Func>(func), ins, args);

    } else {
        constexpr bool possibly_not_bcast = []<size_t... Is>(
                                                std::index_sequence<Is...>) {
            constexpr size_t ref =
                std::tuple_element_t<0, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<0, std::tuple<uinexts_t...>>::rank();
            return (
                (std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                     std::tuple_element_t<Is,
                                          std::tuple<uinexts_t...>>::rank() ==
                 ref) &&
                ...);
        }(std::make_index_sequence<sizeof...(ins_t)>{});

        if constexpr (possibly_not_bcast) {
            // Pass without broadcasting (less computation)
            constexpr size_t brank =
                std::tuple_element_t<0, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<0, std::tuple<uinexts_t...>>::rank();

            const bool same_bexts = [&ins]<size_t... Is>(
                                        std::index_sequence<Is...>) {
                return same(std::make_tuple(
                    slice_from_start<brank>(std::get<Is>(ins).extents())...));
            }(std::make_index_sequence<sizeof...(ins_t)>{});

            if (same_bexts) [[likely]] {
                const bool is_flattable =
                    [&ins]<size_t... Is>(std::index_sequence<Is...>) {
                        return (core::is_reshapable(std::get<Is>(ins)) && ...);
                    }(std::make_index_sequence<sizeof...(ins_t)>{});

                if (is_flattable) [[likely]] {
                    // Flatten batch if possible (simd friendly)
                    constexpr size_t static_bsize =
                        static_size<decltype(slice_from_start<brank>(
                            std::get<0>(ins).extents()))>();
                    const size_t bsize = size(
                        slice_from_start<brank>(std::get<0>(ins).extents()));

                    const auto fins = [&ins, &uinexts, &bsize]<size_t... Is>(
                                          std::index_sequence<Is...>) {
                        return std::tuple{reshape(
                            std::get<Is>(ins),
                            concatenate(extents<size_t, static_bsize>{bsize},
                                        std::get<Is>(uinexts)))...};
                    }(std::make_index_sequence<sizeof...(ins_t)>{});

                    detail::batch_impl<1>(std::forward<Func>(func), fins, args,
                                          mpmode);

                } else {
                    detail::batch_impl<brank>(std::forward<Func>(func), ins,
                                              args, mpmode);
                }

                return;
            }
        }

        // Broadcasting
        const auto bexts = [&ins]<size_t... Is>(std::index_sequence<Is...>) {
            return broadcast(std::make_tuple(
                slice_from_start<
                    std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                    std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()>(
                    std::get<Is>(ins).extents())...));
        }(std::make_index_sequence<sizeof...(ins_t)>{});

        constexpr size_t brank = decltype(bexts)::rank();

        const auto bins = [&ins, &uinexts,
                           &bexts]<size_t... Is>(std::index_sequence<Is...>) {
            return std::tuple{
                broadcast_to(std::get<Is>(ins),
                             concatenate(bexts, std::get<Is>(uinexts)))...};
        }(std::make_index_sequence<sizeof...(ins_t)>{});

        detail::batch_impl<brank>(std::forward<Func>(func), bins, args, mpmode);
    }
}

template <typename Func, mdspan_c... ins_t, extents_c... uinexts_t,
          typename... args_t>
    requires(sizeof...(ins_t) == sizeof...(uinexts_t) - 1)
[[nodiscard]] inline constexpr auto
batch(Func &&func, const std::tuple<ins_t...> &ins,
      const std::tuple<uinexts_t...> &uinexts,
      const std::tuple<args_t...> &args, const MPMode mpmode) noexcept {
    auto out = create_out(ins, uinexts);

    batch(std::forward<Func>(func),
          std::tuple_cat(ins, std::make_tuple(to_mdspan(out))), uinexts, args,
          mpmode);

    return out;
}

template <typename Func, mdspan_c... ins_t, extents_c... uinexts_t,
          typename... args_t>
    requires(sizeof...(ins_t) < sizeof...(uinexts_t) - 1)
[[nodiscard]] inline constexpr auto
batch(Func &&func, const std::tuple<ins_t...> &ins,
      const std::tuple<uinexts_t...> &uinexts,
      const std::tuple<args_t...> &args, const MPMode mpmode) noexcept {
    auto outs = create_out(ins, uinexts);

    [&func, &ins, &outs, &uinexts, &args,
     &mpmode]<size_t... Is>(std::index_sequence<Is...>) {
        batch(std::forward<Func>(func),
              std::tuple_cat(ins,
                             std::make_tuple(to_mdspan(std::get<Is>(outs))...)),
              uinexts, args, mpmode);
    }(std::make_index_sequence<std::tuple_size_v<decltype(outs)>>{});

    return outs;
}

} // namespace core
} // namespace ctmd
