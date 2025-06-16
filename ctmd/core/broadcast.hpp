#pragma once

#include "submdspan.hpp"
#include "type.hpp"

namespace ctmd {
namespace core {

template <size_t offset, size_t rank, extents_c in_t>
[[nodiscard]] inline constexpr auto
slice_with_offset(const in_t &in = in_t{}) noexcept {
    static_assert(in_t::rank() >= offset + rank,
                  "Slice rank exceeds input rank.");

    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<typename in_t::index_type,
                       in_t::static_extent(offset + Is)...>{
            in.extent(offset + Is)...};
    }(std::make_index_sequence<rank>{});
}

template <size_t rank, extents_c in_t>
[[nodiscard]] inline constexpr auto
slice_from_left(const in_t &in = in_t{}) noexcept {
    return slice_with_offset<0, rank>(in);
}

template <size_t rank, extents_c in_t>
[[nodiscard]] inline constexpr auto
slice_from_right(const in_t &in = in_t{}) noexcept {
    return slice_with_offset<in_t::rank() - rank, rank>(in);
}

template <extents_c in1_t, extents_c in2_t, extents_c... ins_t>
[[nodiscard]] inline constexpr auto concatenate(const in1_t &in1 = in1_t{},
                                                const in2_t &in2 = in2_t{},
                                                const ins_t &...ins) noexcept {
    using index_t = std::common_type_t<typename in1_t::index_type,
                                       typename in2_t::index_type>;

    const auto cexts =
        [&]<size_t... Is, size_t... Js>(std::index_sequence<Is...>,
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

template <extents_c in_t>
[[nodiscard]] inline constexpr auto
broadcast(const in_t &in = in_t{}) noexcept {
    return in;
}

template <extents_c in1_t, extents_c in2_t, extents_c... ins_t>
[[nodiscard]] inline constexpr auto broadcast(const in1_t &in1 = in1_t{},
                                              const in2_t &in2 = in2_t{},
                                              const ins_t &...ins) noexcept {
    using index_t = std::common_type_t<typename in1_t::index_type,
                                       typename in2_t::index_type>;
    constexpr size_t brank = std::max(in1_t::rank(), in2_t::rank());

    const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return extents<index_t, detail::broadcast_static_extent<
                                    in1_t, in2_t, Is, brank>()...>{[&]() {
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

template <size_t offset, size_t brank, mdspan_c in_t, extents_c new_bexts_t>
[[nodiscard]] inline constexpr auto
broadcast_to_new(const in_t &in = in_t{},
                 const new_bexts_t &new_bexts = new_bexts_t{}) noexcept {
    static_assert(in_t::rank() >= offset + brank, "Incompatible rank");
    static_assert(new_bexts_t::rank() >= brank);

    if constexpr (brank == new_bexts_t::rank() &&
                  []<size_t... Is>(std::index_sequence<Is...>) {
                      return ((in_t::static_extent(offset + Is) != ctmd::dyn &&
                               in_t::static_extent(offset + Is) ==
                                   new_bexts_t::static_extent(Is)) &&
                              ...);
                  }(std::make_index_sequence<brank>{})) {
        return in;

    } else if constexpr (in_t::rank() == 0) {
        using size_type = typename new_bexts_t::size_type;

        auto new_strides = std::array<size_type, new_bexts_t::rank()>{};

        for (size_t i = 0; i < new_bexts_t::rank(); i++) {
            new_strides[i] = 0;
        }

        return mdspan<typename in_t::element_type, new_bexts_t, layout_stride,
                      typename in_t::accessor_type>{
            in.data_handle(), layout_stride::mapping{new_bexts, new_strides}};

    } else {
        using size_type = std::common_type_t<typename in_t::size_type,
                                             typename new_bexts_t::size_type>;

        auto new_strides =
            std::array<size_type, in_t::rank() - brank + new_bexts_t::rank()>{};

        for (size_t i = 0; i < new_strides.size(); i++) {
            if (i < offset) {
                new_strides[i] = static_cast<size_type>(in.stride(i));

            } else if (i < offset + (new_bexts_t::rank() - brank)) {
                new_strides[i] = 0;

            } else if (i < offset + new_bexts_t::rank()) {
                const size_t j = i - (new_bexts_t::rank() - brank);

                if (in.extent(j) == new_bexts.extent(i - offset)) {
                    new_strides[i] = static_cast<size_type>(in.stride(j));

                } else if (in.extent(j) == 1) {
                    new_strides[i] = 0;

                } else {
                    assert(false);
                }

            } else {
                new_strides[i] = static_cast<size_type>(
                    in.stride(i - (new_bexts_t::rank() - brank)));
            }
        }

        auto new_extents = concatenate(
            slice_from_left<offset>(in.extents()), new_bexts,
            slice_from_right<in_t::rank() - offset - brank>(in.extents()));

        return mdspan<typename in_t::element_type, decltype(new_extents),
                      layout_stride, typename in_t::accessor_type>{
            in.data_handle(), layout_stride::mapping{new_extents, new_strides}};
    }
}

namespace detail {

template <size_t BatchRank, typename Func, mdspan_c in_t, mdspan_c... ins_t>
inline constexpr void batch_impl_none(Func &&func, const in_t &in,
                                      const ins_t &...ins) noexcept {
    if constexpr (BatchRank == 0) {
        std::forward<Func>(func)(in, ins...);

    } else {
        for (typename in_t::index_type i = 0; i < in.extent(0); i++) {
            batch_impl_none<BatchRank - 1>(std::forward<Func>(func),
                                           submdspan_from_left(in, i),
                                           submdspan_from_left(ins, i)...);
        }
    }
}

template <size_t brank, typename Func, size_t offset, size_t... offsets,
          mdspan_c in_t, mdspan_c... ins_t>
inline constexpr void
batch_impl_none_new(Func &&func, std::index_sequence<offset, offsets...>,
                    const in_t &in, const ins_t &...ins) noexcept {
    static_assert(sizeof...(offsets) == sizeof...(ins_t),
                  "Number of offsets must match number of inputs.");

    if constexpr (brank == 0) {
        std::forward<Func>(func)(in, ins...);

    } else {
        for (size_t i = 0; i < in.extent(offset); i++) {
            batch_impl_none_new<brank - 1>(
                std::forward<Func>(func),
                std::index_sequence<offset, offsets...>{},
                submdspan_from_left<offset>(in, i),
                submdspan_from_left<offsets>(ins, i)...);
        }
    }
}

#ifdef _OPENMP

template <size_t BatchRank, typename Func, mdspan_c in_t, mdspan_c... ins_t>
inline constexpr void batch_impl_cpump(Func &&func, const in_t &in,
                                       const ins_t &...ins) noexcept {
    if constexpr (BatchRank == 0) {
        std::forward<Func>(func)(in, ins...);

    } else {
#pragma omp parallel for
        for (typename in_t::index_type i = 0; i < in.extent(0); i++) {
            batch_impl_none<BatchRank - 1>(std::forward<Func>(func),
                                           submdspan_from_left(in, i),
                                           submdspan_from_left(ins, i)...);
        }
    }
}

template <size_t brank, typename Func, size_t offset, size_t... offsets,
          mdspan_c in_t, mdspan_c... ins_t>
inline constexpr void
batch_impl_cpump_new(Func &&func, std::index_sequence<offset, offsets...>,
                     const in_t &in, const ins_t &...ins) noexcept {
    static_assert(sizeof...(offsets) == sizeof...(ins_t),
                  "Number of offsets must match number of inputs.");

    if constexpr (brank == 0) {
        std::forward<Func>(func)(in, ins...);

    } else {
#pragma omp parallel for
        for (size_t i = 0; i < in.extent(offset); i++) {
            batch_impl_none_new<brank - 1>(
                std::forward<Func>(func),
                std::index_sequence<offset, offsets...>{},
                submdspan_from_left<offset>(in, i),
                submdspan_from_left<offsets>(ins, i)...);
        }
    }
}

template <typename Func, size_t uin_rank, size_t... uins_rank, mdspan_c in_t,
          mdspan_c... ins_t>
inline constexpr void
batch_impl_cpump_new(Func &&func, std::index_sequence<uin_rank, uins_rank...>,
                     const in_t &in, const ins_t &...ins) noexcept {
    if constexpr (in_t::rank() == uin_rank) {
        std::forward<Func>(func)(in, ins...);

    } else {
#pragma omp parallel for
        for (typename in_t::index_type i = 0; i < in.extent(0); i++) {
            batch_impl_none_new(std::forward<Func>(func),
                                std::index_sequence<uin_rank, uins_rank...>{},
                                submdspan_from_left(in, i),
                                submdspan_from_left(ins, i)...);
        }
    }
}

#endif

template <size_t BatchRank, typename Func, mdspan_c... ins_t>
inline constexpr void batch_impl(Func &&func, const MPMode mpmode,
                                 const ins_t &...ins) noexcept {
    if (!std::is_constant_evaluated()) [[likely]] {
        if (mpmode == MPMode::CPUMP) [[unlikely]] {
#ifdef _OPENMP
            batch_impl_cpump<BatchRank>(std::forward<Func>(func), ins...);
            return;
#else
            assert(false);
#endif
        }
    }

    batch_impl_none<BatchRank>(std::forward<Func>(func), ins...);
}

template <size_t brank, typename Func, size_t... offsets, mdspan_c... ins_t>
inline constexpr void
batch_impl_new(Func &&func, std::index_sequence<offsets...>,
               const MPMode mpmode, const ins_t &...ins) noexcept {
    if (!std::is_constant_evaluated()) [[likely]] {
        if (mpmode == MPMode::CPUMP) [[unlikely]] {
#ifdef _OPENMP
            batch_impl_cpump_new<brank>(std::forward<Func>(func),
                                        std::index_sequence<offsets...>{},
                                        ins...);
            return;
#else
            assert(false);
#endif
        }
    }

    batch_impl_none_new<brank>(std::forward<Func>(func),
                               std::index_sequence<offsets...>{}, ins...);
}

} // namespace detail

template <typename T, extents_c exts_t>
[[nodiscard]] inline constexpr auto create_out(const exts_t &exts) noexcept {
    if constexpr (exts_t::rank() == 0) {
        return T{};

    } else {
        return ctmd::mdarray<T, exts_t>{exts};
    }
}

template <typename T = int8_t, mdspan_c... ins_t, extents_c... uinexts_t>
    requires(sizeof...(ins_t) == sizeof...(uinexts_t) - 1)
[[nodiscard]] inline constexpr auto
create_out(const std::tuple<ins_t...> &ins,
           const std::tuple<uinexts_t...> &uinexts) noexcept {
    using element_t = std::common_type_t<T, element_type_t<ins_t>...>;

    const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(
            slice_from_left<
                std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()>(
                std::get<Is>(ins).extents())...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    return create_out<element_t>(
        core::concatenate(bexts, std::get<sizeof...(ins_t)>(uinexts)));
}

template <typename T = int8_t, size_t... offsets, size_t... branks,
          size_t uout_offset, extents_c uout_exts_t, mdspan_c... ins_t>
    requires(sizeof...(offsets) == sizeof...(ins_t) &&
             sizeof...(branks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
create_out_new(std::index_sequence<offsets...>, std::index_sequence<branks...>,
               std::integral_constant<size_t, uout_offset>,
               const uout_exts_t &uout_exts, const ins_t &...ins) noexcept {
    using element_t = std::common_type_t<T, element_type_t<ins_t>...>;

    constexpr std::array ofst = {offsets...};
    constexpr std::array br = {branks...};
    auto ins_tuple = std::forward_as_tuple(ins...);

    const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(slice_with_offset<ofst[Is], br[Is]>(
            std::get<Is>(ins_tuple).extents())...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    return create_out<element_t>(core::concatenate(
        slice_from_left<uout_offset>(uout_exts), bexts,
        slice_from_right<uout_exts_t::rank() - uout_offset>(uout_exts)));
}

template <typename T = int8_t, mdspan_c... ins_t, extents_c... uinexts_t>
    requires(sizeof...(ins_t) < sizeof...(uinexts_t) - 1)
[[nodiscard]] inline constexpr auto
create_out(const std::tuple<ins_t...> &ins,
           const std::tuple<uinexts_t...> &uinexts) noexcept {
    using element_t = std::common_type_t<T, element_type_t<ins_t>...>;

    const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(
            slice_from_left<
                std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()>(
                std::get<Is>(ins).extents())...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return std::tuple{create_out<element_t>(core::concatenate(
            bexts, std::get<sizeof...(ins_t) + Is>(uinexts)))...};
    }(std::make_index_sequence<sizeof...(uinexts_t) - sizeof...(ins_t)>{});
}

template <typename Func, mdspan_c... ins_t, extents_c... uinexts_t>
    requires(sizeof...(ins_t) == sizeof...(uinexts_t))
inline constexpr void batch(Func &&func, const std::tuple<ins_t...> &ins,
                            const std::tuple<uinexts_t...> &uinexts,
                            const MPMode mpmode) noexcept {
    constexpr bool need_batch = []<size_t... Is>(std::index_sequence<Is...>) {
        return ((std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() !=
                 std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()) ||
                ...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    if constexpr (!need_batch) {
        // Pass directly to the function
        std::apply(
            [&](auto &&...elems) {
                detail::batch_impl_none<0>(
                    std::forward<Func>(func),
                    std::forward<decltype(elems)>(elems)...);
            },
            ins);

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

            const bool same_bexts = [&]<size_t... Is>(
                                        std::index_sequence<Is...>) {
                return same(std::make_tuple(
                    slice_from_left<brank>(std::get<Is>(ins).extents())...));
            }(std::make_index_sequence<sizeof...(ins_t)>{});

            if (same_bexts) [[likely]] {
                const bool is_flattable =
                    [&]<size_t... Is>(std::index_sequence<Is...>) {
                        return (core::is_reshapable(std::get<Is>(ins)) && ...);
                    }(std::make_index_sequence<sizeof...(ins_t)>{});

                if (is_flattable) [[likely]] {
                    // Flatten batch if possible (simd friendly)
                    constexpr size_t static_bsize =
                        static_size<decltype(slice_from_left<brank>(
                            std::get<0>(ins).extents()))>();
                    const size_t bsize = size(
                        slice_from_left<brank>(std::get<0>(ins).extents()));

                    const auto fins = [&]<size_t... Is>(
                                          std::index_sequence<Is...>) {
                        return std::tuple{reshape(
                            std::get<Is>(ins),
                            concatenate(extents<size_t, static_bsize>{bsize},
                                        std::get<Is>(uinexts)))...};
                    }(std::make_index_sequence<sizeof...(ins_t)>{});

                    std::apply(
                        [&](auto &&...elems) {
                            detail::batch_impl<1>(
                                std::forward<Func>(func), mpmode,
                                std::forward<decltype(elems)>(elems)...);
                        },
                        fins);

                } else {
                    std::apply(
                        [&](auto &&...elems) {
                            detail::batch_impl<brank>(
                                std::forward<Func>(func), mpmode,
                                std::forward<decltype(elems)>(elems)...);
                        },
                        ins);
                }

                return;
            }
        }

        // Broadcasting
        const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
            return broadcast(
                slice_from_left<
                    std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
                    std::tuple_element_t<Is, std::tuple<uinexts_t...>>::rank()>(
                    std::get<Is>(ins).extents())...);
        }(std::make_index_sequence<sizeof...(ins_t)>{});

        constexpr size_t brank = decltype(bexts)::rank();

        const auto bins = [&]<size_t... Is>(std::index_sequence<Is...>) {
            return std::tuple{
                broadcast_to(std::get<Is>(ins),
                             concatenate(bexts, std::get<Is>(uinexts)))...};
        }(std::make_index_sequence<sizeof...(ins_t)>{});

        std::apply(
            [&](auto &&...elems) {
                detail::batch_impl<brank>(
                    std::forward<Func>(func), mpmode,
                    std::forward<decltype(elems)>(elems)...);
            },
            bins);
    }
}

template <typename Func, size_t... offsets, size_t... branks, mdspan_c... ins_t>
    requires(sizeof...(offsets) == sizeof...(ins_t) &&
             sizeof...(branks) == sizeof...(ins_t))
inline constexpr void batch_new(Func &&func, std::index_sequence<offsets...>,
                                std::index_sequence<branks...>,
                                const MPMode mpmode,
                                const ins_t &...ins) noexcept {
    constexpr std::array ofst = {offsets...};
    constexpr std::array br = {branks...};
    auto ins_tuple = std::forward_as_tuple(ins...);

    const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(slice_with_offset<ofst[Is], br[Is]>(
            std::get<Is>(ins_tuple).extents())...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        detail::batch_impl_new<bexts.rank()>(
            std::forward<Func>(func), std::index_sequence<offsets...>{}, mpmode,
            broadcast_to_new<ofst[Is], br[Is]>(std::get<Is>(ins_tuple),
                                               bexts)...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});
}

template <typename T = int8_t, typename Func, mdspan_c... ins_t,
          extents_c... uinexts_t>
    requires(sizeof...(ins_t) == sizeof...(uinexts_t) - 1)
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, const std::tuple<ins_t...> &ins,
          const std::tuple<uinexts_t...> &uinexts,
          const MPMode mpmode) noexcept {
    auto out = create_out<T>(ins, uinexts);

    batch(std::forward<Func>(func),
          std::tuple_cat(ins, std::make_tuple(to_mdspan(out))), uinexts,
          mpmode);

    return out;
}

template <typename T = int8_t, typename Func, size_t... offsets,
          size_t... branks, size_t uout_offset, extents_c uout_exts_t,
          mdspan_c... ins_t>
    requires(sizeof...(offsets) == sizeof...(ins_t) &&
             sizeof...(branks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
batch_out_new(Func &&func, std::index_sequence<offsets...>,
              std::index_sequence<branks...>,
              std::integral_constant<size_t, uout_offset>,
              const uout_exts_t &uout_exts, const MPMode mpmode,
              const ins_t &...ins) noexcept {
    auto out = create_out_new<T>(
        std::index_sequence<offsets...>{}, std::index_sequence<branks...>{},
        std::integral_constant<size_t, uout_offset>{}, uout_exts, ins...);

    batch_new(std::forward<Func>(func),
              std::index_sequence<offsets..., uout_offset>{},
              std::index_sequence<branks..., std::max(branks...)>{}, mpmode,
              ins..., core::to_mdspan(out));

    return out;
}

template <typename T = int8_t, typename Func, mdspan_c... ins_t,
          extents_c... uinexts_t>
    requires(sizeof...(ins_t) < sizeof...(uinexts_t) - 1)
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, const std::tuple<ins_t...> &ins,
          const std::tuple<uinexts_t...> &uinexts,
          const MPMode mpmode) noexcept {
    auto outs = create_out<T>(ins, uinexts);

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        batch(std::forward<Func>(func),
              std::tuple_cat(ins,
                             std::make_tuple(to_mdspan(std::get<Is>(outs))...)),
              uinexts, mpmode);
    }(std::make_index_sequence<std::tuple_size_v<decltype(outs)>>{});

    return outs;
}

} // namespace core
} // namespace ctmd
