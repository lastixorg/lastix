#pragma once

#include <cstdint>

namespace lx::core {

    using u8 = uint8_t;
    using i8 = int8_t;
    using u16 = uint16_t;
    using i16 = int16_t;
    using u32 = uint32_t;
    using i32 = int32_t;
    using u64 = uint64_t;
    using i64 = int64_t;
    using usize = std::size_t;

#if __SIZEOF_FLOAT__ == 4
    using f32 = float;
#endif

#if __SIZEOF_DOUBLE__ == 8
    using f64 = double;
#endif

#if __SIZEOF_LONG_DOUBLE__ == 16
    using f128 = long double;
#endif

}; // namespace lx::core
