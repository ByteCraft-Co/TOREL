#pragma once

#include <compare>
#include <cstdint>
#include <limits>

namespace torel::source {

class SourceId final {
public:
    using Value = std::uint32_t;

    static constexpr Value invalid_value =
        std::numeric_limits<Value>::max();

    constexpr SourceId() noexcept = default;

    explicit constexpr SourceId(Value value) noexcept
        : value_(value)
    {
    }

    [[nodiscard]]
    static constexpr SourceId invalid() noexcept
    {
        return SourceId{};
    }

    [[nodiscard]]
    constexpr bool is_valid() const noexcept
    {
        return value_ != invalid_value;
    }

    [[nodiscard]]
    constexpr Value value() const noexcept
    {
        return value_;
    }

    auto operator<=>(const SourceId&) const = default;

private:
    Value value_ = invalid_value;
};

} // namespace torel::source