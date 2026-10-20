#pragma once

#include <compare>
#include <cstdint>
#include <limits>

namespace torel::hir {

class NodeId final {
public:
    using Value = std::uint32_t;

    static constexpr Value invalid_value =
        std::numeric_limits<Value>::max();

    constexpr NodeId() noexcept = default;

    explicit constexpr NodeId(Value value) noexcept
        : value_(value)
    {
    }

    [[nodiscard]]
    static constexpr NodeId invalid() noexcept
    {
        return NodeId{};
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

    auto operator<=>(const NodeId&) const = default;

private:
    Value value_ = invalid_value;
};

} // namespace torel::hir