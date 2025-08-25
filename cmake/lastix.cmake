# Common for GCC and Clang
set(
    common_warnings
    -Wall
    -Wextra
    -pedantic
    -Wcast-align
    -Wcast-qual
    -Wsign-promo
    -Wdisabled-optimization
    -Wunreachable-code
    -Wunused
    -Wuninitialized
    -Wshadow
    -Wfloat-equal
    -Wctor-dtor-privacy
    -Winit-self
    -Wmissing-declarations
    -Wmissing-include-dirs
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-conversion
    -Wstrict-overflow=5
    -ftemplate-backtrace-limit=1
    -fno-exceptions
    -fno-rtti
)

add_library(
    lastix.warnings INTERFACE
)

target_compile_options(
    lastix.warnings INTERFACE

    $<$<CXX_COMPILER_ID:GNU>:${common_warnings}
    -Wnoexcept -Wlogical-op -Wstrict-null-sentinel>

    $<$<CXX_COMPILER_ID:Clang>:${common_warnings}
    -Wzero-as-null-pointer-constant -Wdouble-promotion -Wcomma -Wextra-semi>

    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /w14242
    /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311
    /w14545 /w14546 /w14547 /w14549 /w14555 /w14619 /w14640>
)

# Add library/executable helpers
function(lastix_add_library target)
    add_library(${target} STATIC ${ARGN})
    set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(${target} PRIVATE lastix.warnings)
endfunction()

function(lastix_add_executable target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} PRIVATE lastix.warnings)
endfunction()
