#pragma once

#include <source_location>
#include <string_view>

namespace lx::core {

    [[noreturn]] auto
        panic(std::string_view msg,
              std::source_location loc = std::source_location::current())
            -> void;

};
