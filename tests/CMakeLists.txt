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
    "core/box.cpp"
)

target_link_libraries(
    lastix-tests PRIVATE
    lastix::core
    Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(
    lastix-tests
)
