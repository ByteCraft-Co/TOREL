#pragma once

#include <compare>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace torel::diagnostics {

class DiagnosticCode final {
public:
    explicit DiagnosticCode(std::string value)
        : value_(std::move(value))
    {
        if (value_.empty()) {
            throw std::invalid_argument{
                "TOREL diagnostic code cannot be empty"
            };
        }
    }

    [[nodiscard]]
    std::string_view value() const noexcept
    {
        return value_;
    }

    auto operator<=>(const DiagnosticCode&) const = default;

private:
    std::string value_;
};

} // namespace torel::diagnostics