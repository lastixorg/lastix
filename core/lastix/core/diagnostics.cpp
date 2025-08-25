#ifndef LASTIX_NO_OS_ASSUMPTIONS
#include "lastix/core/diagnostics.hpp"

#ifndef LASTIX_CORE_NO_PANIC
#include <print>
#include <cstdlib>

auto lx::core::panic(std::string_view msg, std::source_location loc) -> void {
    std::println("{}:{} {} {}", loc.file_name(), loc.line(),
                 loc.function_name(), msg);
    std::abort();
}

#endif
#endif
