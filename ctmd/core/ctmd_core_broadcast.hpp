#pragma once

#include <optional>

#include "ctmd_core_submdspan.hpp"
#include "ctmd_core_type.hpp"

namespace ctmd {
namespace core {

template <size_t offset, size_t rank, extents_c in_t>
[[nodiscard]] inline constexpr auto
slice_with_offset(const in_t &in = in_t{}) noexcept {
    static_assert(in_t::rank() >= offset + rank,
                  "Incompatible offset and rank for slicing.");

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

template <size_t offset, size_t brank, mdspan_c in_t, extents_c new_bexts_t>
[[nodiscard]] inline constexpr auto
broadcast_to(const in_t &in = in_t{},
             const new_bexts_t &new_bexts = new_bexts_t{}) noexcept {
    constexpr size_t urank = in_t::rank() - brank;

    static_assert(in_t::rank() >= offset + brank,
                  "Incompatible offset and brank for broadcasting.");
    static_assert(new_bexts_t::rank() >= brank,
                  "Incompatible brank for broadcasting.");

    if constexpr (brank == new_bexts_t::rank() &&
                  []<size_t... Is>(std::index_sequence<Is...>) {
                      return ((in_t::static_extent(offset + Is) != ctmd::dyn &&
                               in_t::static_extent(offset + Is) ==
                                   new_bexts_t::static_extent(Is)) &&
                              ...);
                  }(std::make_index_sequence<brank>{})) {
        return in;

    } else if constexpr (in_t::rank() == 0) {
        using index_type = typename new_bexts_t::index_type;

        auto new_strides = std::array<index_type, new_bexts_t::rank()>{};

        for (size_t i = 0; i < new_bexts_t::rank(); i++) {
            new_strides[i] = 0;
        }

        return mdspan<typename in_t::element_type, new_bexts_t, layout_stride,
                      typename in_t::accessor_type>{
            in.data_handle(), layout_stride::mapping{new_bexts, new_strides}};

    } else {
        using index_type = std::common_type_t<typename in_t::index_type,
                                              typename new_bexts_t::index_type>;

        auto new_strides =
            std::array<index_type, urank + new_bexts_t::rank()>{};

        for (size_t i = 0; i < new_strides.size(); i++) {
            if (i < offset) {
                new_strides[i] = static_cast<index_type>(in.stride(i));

            } else if (i < offset + (new_bexts_t::rank() - brank)) {
                new_strides[i] = 0;

            } else if (i < offset + new_bexts_t::rank()) {
                const size_t j = i - (new_bexts_t::rank() - brank);

                if (static_cast<size_t>(in.extent(j)) ==
                    static_cast<size_t>(new_bexts.extent(i - offset))) {
                    new_strides[i] = static_cast<index_type>(in.stride(j));

                } else if (in.extent(j) == 1) {
                    new_strides[i] = 0;

                } else {
                    assert(false);
                }

            } else {
                new_strides[i] = static_cast<index_type>(
                    in.stride(i - (new_bexts_t::rank() - brank)));
            }
        }

        const auto new_extents = [&]() {
            if constexpr (offset == 0) {
                return concatenate(new_bexts,
                                   slice_from_right<urank>(in.extents()));

            } else {
                return concatenate(
                    slice_from_left<offset>(in.extents()), new_bexts,
                    slice_from_right<urank - offset>(in.extents()));
            }
        }();

        return mdspan<typename in_t::element_type,
                      std::remove_const_t<decltype(new_extents)>, layout_stride,
                      typename in_t::accessor_type>{
            in.data_handle(), layout_stride::mapping{new_extents, new_strides}};
    }
}

namespace detail {

template <size_t brank, typename Func, size_t offset, size_t... offsets,
          mdspan_c in_t, mdspan_c... ins_t>
inline constexpr void
batch_impl_none(Func &&func, std::index_sequence<offset, offsets...>,
                const in_t &in, const ins_t &...ins) noexcept {
    static_assert(sizeof...(offsets) == sizeof...(ins_t),
                  "Number of offsets must match number of inputs.");

    if constexpr (brank == 0) {
        std::forward<Func>(func)(in, ins...);

    } else {
        for (typename in_t::index_type i = 0; i < in.extent(offset); i++) {
            batch_impl_none<brank - 1>(
                std::forward<Func>(func),
                std::index_sequence<offset, offsets...>{},
                submdspan_from_left<offset>(in, i),
                submdspan_from_left<offsets>(ins, i)...);
        }
    }
}

#if defined(_OPENMP) && defined(REAL_GCC)

template <size_t brank, typename Func, size_t offset, size_t... offsets,
          mdspan_c in_t, mdspan_c... ins_t>
inline void batch_impl_cpump(Func &&func,
                             std::index_sequence<offset, offsets...>,
                             const in_t &in, const ins_t &...ins) noexcept {
    static_assert(sizeof...(offsets) == sizeof...(ins_t),
                  "Number of offsets must match number of inputs.");

    if constexpr (brank == 0) {
        std::forward<Func>(func)(in, ins...);

    } else {
#pragma omp parallel for
        for (typename in_t::index_type i = 0; i < in.extent(offset); i++) {
            batch_impl_none<brank - 1>(
                std::forward<Func>(func),
                std::index_sequence<offset, offsets...>{},
                submdspan_from_left<offset>(in, i),
                submdspan_from_left<offsets>(ins, i)...);
        }
    }
}

#endif

template <size_t brank, typename Func, size_t... offsets, mdspan_c... ins_t>
inline constexpr void batch_impl(Func &&func, std::index_sequence<offsets...>,
                                 const ctmd::MPMode mpmode,
                                 const ins_t &...ins) noexcept {
    static_assert(sizeof...(offsets) == sizeof...(ins_t),
                  "Number of offsets must match number of inputs.");

    if (!std::is_constant_evaluated()) [[likely]] {
        if (mpmode == ctmd::MPMode::CPUMP) [[unlikely]] {
// TODO: use cpump in clang
#if defined(_OPENMP) && defined(REAL_GCC)
            batch_impl_cpump<brank>(std::forward<Func>(func),
                                    std::index_sequence<offsets...>{}, ins...);
            return;
#else
            assert(false);
#endif
        }
    }

    batch_impl_none<brank>(std::forward<Func>(func),
                           std::index_sequence<offsets...>{}, ins...);
}

} // namespace detail

template <typename dtype, extents_c exts_t>
[[nodiscard]] inline constexpr auto create_out(const exts_t &exts) noexcept {
    if constexpr (exts_t::rank() == 0) {
        return dtype{};

    } else {
        return ctmd::mdarray<dtype, exts_t>{exts};
    }
}

namespace detail {

template <typename... Ts> struct filter_nullopt;

template <> struct filter_nullopt<> {
    using type = std::tuple<>;
};

template <typename T, typename... Ts> struct filter_nullopt<T, Ts...> {
  private:
    using Tail = typename filter_nullopt<Ts...>::type;

  public:
    using type = std::conditional_t<
        std::is_same_v<std::remove_cvref_t<T>, std::nullopt_t>, Tail,
        decltype(std::tuple_cat(std::declval<std::tuple<T>>(),
                                std::declval<Tail>()))>;
};

template <typename Tuple> struct tuple_common_type;

template <typename... Ts> struct tuple_common_type<std::tuple<Ts...>> {
    using type = std::common_type_t<Ts...>;
};

} // namespace detail

template <typename... Ts>
using common_type_t = typename detail::tuple_common_type<
    typename detail::filter_nullopt<Ts...>::type>::type;

template <typename dtype = void, size_t... offsets, size_t... uranks,
          extents_c uout_exts_t, mdspan_c... ins_t>
    requires(sizeof...(offsets) == sizeof...(ins_t) + 1 &&
             sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
create_out(std::index_sequence<offsets...>, std::index_sequence<uranks...>,
           const uout_exts_t &uout_exts, const ins_t &...ins) noexcept {
    using value_t = std::conditional_t<!std::is_void_v<dtype>, dtype,
                                       common_type_t<value_type_t<ins_t>...>>;

    constexpr auto ofst = std::array{offsets...};
    constexpr auto ur = std::array{uranks...};
    constexpr auto br = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return std::array{
            (std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
             ur[Is])...};
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    auto ins_tuple = std::forward_as_tuple(ins...);

    const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(slice_with_offset<ofst[Is], br[Is]>(
            std::get<Is>(ins_tuple).extents())...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    constexpr size_t uout_offset = ofst[sizeof...(ins_t)];

    return create_out<value_t>(core::concatenate(
        slice_from_left<uout_offset>(uout_exts), bexts,
        slice_from_right<uout_exts_t::rank() - uout_offset>(uout_exts)));
}

template <typename dtype = void, size_t... uranks, extents_c uout_exts_t,
          mdspan_c... ins_t>
    requires(sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto create_out(std::index_sequence<uranks...>,
                                               const uout_exts_t &uout_exts,
                                               const ins_t &...ins) noexcept {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return create_out<dtype>(std::index_sequence<((void)Is, 0)...>{},
                                 std::index_sequence<uranks...>{}, uout_exts,
                                 ins...);
    }(std::make_index_sequence<sizeof...(ins_t) + 1>{});
}

template <typename dtype = void, extents_c uout_exts_t, mdspan_c... ins_t>
[[nodiscard]] inline constexpr auto create_out(const uout_exts_t &uout_exts,
                                               const ins_t &...ins) noexcept {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return create_out<dtype>(std::index_sequence<((void)Is, 0)...>{},
                                 uout_exts, ins...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});
}

template <typename dtype = void, size_t... offsets, size_t... uranks,
          extents_c... uouts_exts_t, mdspan_c... ins_t>
    requires(sizeof...(offsets) == sizeof...(ins_t) + sizeof...(uouts_exts_t) &&
             sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
create_out(std::index_sequence<offsets...>, std::index_sequence<uranks...>,
           const std::tuple<uouts_exts_t...> &uouts_exts,
           const ins_t &...ins) noexcept {
    using value_t = std::conditional_t<!std::is_void_v<dtype>, dtype,
                                       common_type_t<value_type_t<ins_t>...>>;

    constexpr auto ofst = std::array{offsets...};
    constexpr auto ur = std::array{uranks...};
    constexpr auto br = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return std::array{
            (std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
             ur[Is])...};
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    auto ins_tuple = std::forward_as_tuple(ins...);

    const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return broadcast(slice_with_offset<ofst[Is], br[Is]>(
            std::get<Is>(ins_tuple).extents())...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return std::tuple{create_out<value_t>(core::concatenate(
            slice_from_left<ofst[sizeof...(ins_t) + Is]>(
                std::get<Is>(uouts_exts)),
            bexts,
            slice_from_right<
                std::tuple_element_t<Is, std::tuple<uouts_exts_t...>>::rank() -
                ofst[sizeof...(ins_t) + Is]>(std::get<Is>(uouts_exts))))...};
    }(std::make_index_sequence<sizeof...(uouts_exts_t)>{});
}

template <typename dtype = void, size_t... uranks, extents_c... uouts_exts_t,
          mdspan_c... ins_t>
    requires(sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
create_out(std::index_sequence<uranks...>,
           const std::tuple<uouts_exts_t...> &uouts_exts,
           const ins_t &...ins) noexcept {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return create_out<dtype>(std::index_sequence<((void)Is, 0)...>{},
                                 std::index_sequence<uranks...>{}, uouts_exts,
                                 ins...);
    }(std::make_index_sequence<sizeof...(ins_t) + sizeof...(uouts_exts_t)>{});
}

template <typename dtype = void, size_t... uranks, extents_c... uouts_exts_t,
          mdspan_c... ins_t>
    requires(sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
create_out(const std::tuple<uouts_exts_t...> &uouts_exts,
           const ins_t &...ins) noexcept {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return create_out<dtype>(std::index_sequence<((void)Is, 0)...>{},
                                 uouts_exts, ins...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});
}

template <typename Func, size_t... offsets, size_t... uranks, mdspan_c... ins_t>
    requires(sizeof...(offsets) == sizeof...(ins_t) &&
             sizeof...(uranks) == sizeof...(ins_t))
inline constexpr void batch(Func &&func, std::index_sequence<offsets...>,
                            std::index_sequence<uranks...>,
                            const ctmd::MPMode mpmode,
                            const ins_t &...ins) noexcept {
    constexpr auto ofst = std::array{offsets...};
    constexpr auto ur = std::array{uranks...};
    constexpr auto br = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return std::array{
            (std::tuple_element_t<Is, std::tuple<ins_t...>>::rank() -
             ur[Is])...};
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    auto ins_tuple = std::forward_as_tuple(ins...);

    constexpr bool no_branks = [&]<size_t... Is>(std::index_sequence<Is...>) {
        return ((br[Is] == 0) && ...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});

    if constexpr (no_branks) {
        // If all branks are 0, batch is not required.
        std::forward<Func>(func)(ins...);

    } else {
        constexpr bool possibly_same_bexts =
            [&]<size_t... Is>(std::index_sequence<Is...>) {
                return ((br[Is] == br[0]) && ...);
            }(std::make_index_sequence<sizeof...(ins_t)>{});

        if constexpr (possibly_same_bexts) {
            const auto same_bexts =
                [&]<size_t... Is>(std::index_sequence<Is...>) {
                    return core::same(slice_with_offset<ofst[Is], br[Is]>(
                        std::get<Is>(ins_tuple).extents())...);
                }(std::make_index_sequence<sizeof...(ins_t)>{});

            if (same_bexts) [[likely]] {
                // If all bexts are same, broadcasting is not required.

                const bool is_flattable =
                    [&]<size_t... Is>(std::index_sequence<Is...>) {
                        return (core::is_reshapable(std::get<Is>(ins_tuple)) &&
                                ...);
                    }(std::make_index_sequence<sizeof...(ins_t)>{});

                if (is_flattable) [[likely]] {
                    // If flattable, reshape to use single for loop (simd
                    // friendly)

                    const auto bexts = slice_with_offset<ofst[0], br[0]>(
                        std::get<0>(ins_tuple).extents());

                    constexpr size_t static_bsize = core::static_size(bexts);
                    const size_t bsize = core::size(bexts);

                    [&]<size_t... Is>(std::index_sequence<Is...>) {
                        detail::batch_impl<1>(
                            std::forward<Func>(func),
                            std::index_sequence<offsets...>{}, mpmode,
                            core::reshape(
                                std::get<Is>(ins_tuple),
                                concatenate(
                                    slice_from_left<ofst[Is]>(
                                        std::get<Is>(ins_tuple).extents()),
                                    extents<size_t, static_bsize>{bsize},
                                    slice_from_right<
                                        std::tuple_element_t<
                                            Is, std::tuple<ins_t...>>::rank() -
                                        ofst[Is] - br[Is]>(
                                        std::get<Is>(ins_tuple)
                                            .extents())))...);
                    }(std::make_index_sequence<sizeof...(ins_t)>{});

                } else {
                    // If not flattable, pass to nested for loop
                    detail::batch_impl<br[0]>(std::forward<Func>(func),
                                              std::index_sequence<offsets...>{},
                                              mpmode, ins...);
                }

                return;
            }
        }

        // If all previous conditions are false, broadcasting is required.
        const auto bexts = [&]<size_t... Is>(std::index_sequence<Is...>) {
            return broadcast(slice_with_offset<ofst[Is], br[Is]>(
                std::get<Is>(ins_tuple).extents())...);
        }(std::make_index_sequence<sizeof...(ins_t)>{});

        [&]<size_t... Is>(std::index_sequence<Is...>) {
            detail::batch_impl<bexts.rank()>(
                std::forward<Func>(func), std::index_sequence<offsets...>{},
                mpmode,
                broadcast_to<ofst[Is], br[Is]>(std::get<Is>(ins_tuple),
                                               bexts)...);
        }(std::make_index_sequence<sizeof...(ins_t)>{});
    }
}

template <typename Func, size_t... uranks, mdspan_c... ins_t>
    requires(sizeof...(uranks) == sizeof...(ins_t))
inline constexpr void batch(Func &&func, std::index_sequence<uranks...>,
                            const ctmd::MPMode mpmode,
                            const ins_t &...ins) noexcept {
    [&]<size_t... Is>(std::index_sequence<Is...>) {
        batch(std::forward<Func>(func), std::index_sequence<((void)Is, 0)...>{},
              std::index_sequence<uranks...>{}, mpmode, ins...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});
}

template <typename Func, mdspan_c... ins_t>
inline constexpr void batch(Func &&func, const ctmd::MPMode mpmode,
                            const ins_t &...ins) noexcept {
    [&]<size_t... Is>(std::index_sequence<Is...>) {
        batch(std::forward<Func>(func), std::index_sequence<((void)Is, 0)...>{},
              mpmode, ins...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});
}

template <typename dtype = void, typename Func, size_t... offsets,
          size_t... uranks, extents_c uout_exts_t, mdspan_c... ins_t>
    requires(sizeof...(offsets) == sizeof...(ins_t) + 1 &&
             sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, std::index_sequence<offsets...>,
          std::index_sequence<uranks...>, const uout_exts_t &uout_exts,
          const ctmd::MPMode mpmode, const ins_t &...ins) noexcept {
    auto out =
        create_out<dtype>(std::index_sequence<offsets...>{},
                          std::index_sequence<uranks...>{}, uout_exts, ins...);

    batch(std::forward<Func>(func), std::index_sequence<offsets...>{},
          std::index_sequence<uranks..., uout_exts_t::rank()>{}, mpmode, ins...,
          core::to_mdspan(out));

    return out;
}

template <typename dtype = void, typename Func, size_t... uranks,
          extents_c uout_exts_t, mdspan_c... ins_t>
    requires(sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, std::index_sequence<uranks...>,
          const uout_exts_t &uout_exts, const ctmd::MPMode mpmode,
          const ins_t &...ins) noexcept {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return batch_out<dtype>(
            std::forward<Func>(func), std::index_sequence<((void)Is, 0)...>{},
            std::index_sequence<uranks...>{}, uout_exts, mpmode, ins...);
    }(std::make_index_sequence<sizeof...(ins_t) + 1>{});
}

template <typename dtype = void, typename Func, extents_c uout_exts_t,
          mdspan_c... ins_t>
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, const uout_exts_t &uout_exts, const ctmd::MPMode mpmode,
          const ins_t &...ins) noexcept {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return batch_out<dtype>(std::forward<Func>(func),
                                std::index_sequence<((void)Is, 0)...>{},
                                uout_exts, mpmode, ins...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});
}

template <typename dtype = void, typename Func, size_t... offsets,
          size_t... uranks, extents_c... uouts_exts_t, mdspan_c... ins_t>
    requires(sizeof...(offsets) == sizeof...(ins_t) + sizeof...(uouts_exts_t) &&
             sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, std::index_sequence<offsets...>,
          std::index_sequence<uranks...>,
          const std::tuple<uouts_exts_t...> &uouts_exts,
          const ctmd::MPMode mpmode, const ins_t &...ins) noexcept {
    auto out =
        create_out<dtype>(std::index_sequence<offsets...>{},
                          std::index_sequence<uranks...>{}, uouts_exts, ins...);

    [&]<size_t... Is>(std::index_sequence<Is...>) {
        batch(std::forward<Func>(func), std::index_sequence<offsets...>{},
              std::index_sequence<
                  uranks..., std::tuple_element_t<
                                 Is, std::tuple<uouts_exts_t...>>::rank()...>{},
              mpmode, ins..., core::to_mdspan(std::get<Is>(out))...);
    }(std::make_index_sequence<sizeof...(uouts_exts_t)>{});

    return out;
}

template <typename dtype = void, typename Func, size_t... uranks,
          extents_c... uouts_exts_t, mdspan_c... ins_t>
    requires(sizeof...(uranks) == sizeof...(ins_t))
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, std::index_sequence<uranks...>,
          const std::tuple<uouts_exts_t...> &uouts_exts,
          const ctmd::MPMode mpmode, const ins_t &...ins) noexcept {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return batch_out<dtype>(
            std::forward<Func>(func), std::index_sequence<((void)Is, 0)...>{},
            std::index_sequence<uranks...>{}, uouts_exts, mpmode, ins...);
    }(std::make_index_sequence<sizeof...(ins_t) + sizeof...(uouts_exts_t)>{});
}

template <typename dtype = void, typename Func, extents_c... uouts_exts_t,
          mdspan_c... ins_t>
[[nodiscard]] inline constexpr auto
batch_out(Func &&func, const std::tuple<uouts_exts_t...> &uouts_exts,
          const ctmd::MPMode mpmode, const ins_t &...ins) noexcept {
    return [&]<size_t... Is>(std::index_sequence<Is...>) {
        return batch_out<dtype>(std::forward<Func>(func),
                                std::index_sequence<((void)Is, 0)...>{},
                                uouts_exts, mpmode, ins...);
    }(std::make_index_sequence<sizeof...(ins_t)>{});
}

} // namespace core
} // namespace ctmd
