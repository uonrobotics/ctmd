#include <gtest/gtest.h>

#include "ctmd/ctmd_any.hpp"
#include "ctmd/ctmd_array_equal.hpp"

namespace md = ctmd;

TEST(test, 1) {
    static_assert(md::array_equal(
        md::any(md::mdarray<uint8_t, md::extents<uint8_t, 2, 2>>{
            std::array<uint8_t, 4>{true, false, true, false}}),
        true));
}
