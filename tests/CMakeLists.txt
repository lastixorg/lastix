CPMAddPackage(
    NAME Catch2
    GITHUB_REPOSITORY catchorg/Catch2
    VERSION 3.10.0
)

list(
    APPEND CMAKE_MODULE_PATH
    "${Catch2_SOURCE_DIR}/extras"
)

add_executable(
    lastix-tests
    "main.cpp"
    "core/arc.cpp"
    "core/box.cpp"
    "core/memory_helpers.cpp"
    "core/memory_helpers.hpp"
    "core/result.cpp"
)

target_compile_options(lastix-tests PRIVATE
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined
    -fno-sanitize-recover=all
    -fno-omit-frame-pointer
    -g3
)

target_link_libraries(
    lastix-tests PRIVATE
    lastix::core
    Catch2::Catch2WithMain
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=undefined
)

include(Catch)

catch_discover_tests(
    lastix-tests
)
