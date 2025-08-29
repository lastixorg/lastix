lastix_add_library(
    lastix.core
    "lastix/core/arc.hpp"
    "lastix/core/box.hpp"
    "lastix/core/diagnostics.hpp"
    "lastix/core/diagnostics.cpp"
    "lastix/core/error.cpp"
    "lastix/core/error.hpp"
    "lastix/core/memory.hpp"
    "lastix/core/option.hpp"
    "lastix/core/result.hpp"
    "lastix/trait/sync.hpp"
)

target_include_directories(
    lastix.core PUBLIC
    "${PROJECT_SOURCE_DIR}/core"
)

add_library(
    lastix::core ALIAS
    lastix.core
)
