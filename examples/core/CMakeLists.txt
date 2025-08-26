lastix_add_executable(
    example-core-box
    "box.cpp"
)

target_link_libraries(
    example-core-box PRIVATE
    lastix::core
)

lastix_add_executable(
    example-core-option
    "option.cpp"
)

target_link_libraries(
    example-core-option PRIVATE
    lastix::core
)
