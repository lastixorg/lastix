lastix_add_executable(
    example-core-box
    "box.cpp"
)

target_link_libraries(
    example-core-box PRIVATE
    lastix::core
)
