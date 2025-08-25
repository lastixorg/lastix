lastix_add_library(
    lastix.core
    "lastix/core/diagnostics.hpp"
    "lastix/core/diagnostics.cpp"
)

target_include_directories(
    lastix.core PUBLIC
    "${PROJECT_SOURCE_DIR}/core"
)

add_library(
    lastix::core ALIAS
    lastix.core
)
