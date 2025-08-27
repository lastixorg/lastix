#include "lastix/core/error.hpp"

namespace lx::core {

    namespace impl {

        StringError::StringError(std::string err) : _err(std::move(err)) {
        }

        auto StringError::what() const noexcept -> std::string_view {
            return _err;
        }

    }; // namespace impl

    auto Error::context(std::string_view msg) noexcept -> Error {
        auto err = Error(std::string(msg));
        err._next = Some(Box<Error>(std::move(*this)));
        return err;
    }

    auto Error::what() const noexcept -> std::string_view {
        return _inner->what();
    }

}; // namespace lx::core
