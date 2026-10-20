#pragma once

#include "torel/source/SourceLocation.hpp"

#include <compare>

namespace torel::source {

struct SourceSpan final {
    SourceId source{};
    ByteOffset begin = 0;
    ByteOffset end = 0;

    [[nodiscard]]
    constexpr bool is_valid() const noexcept
    {
        return source.is_valid() && begin <= end;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept
    {
        return begin == end;
    }

    [[nodiscard]]
    constexpr ByteOffset size_bytes() const noexcept
    {
        return end >= begin
            ? end - begin
            : 0;
    }

    [[nodiscard]]
    constexpr SourceLocation begin_location() const noexcept
    {
        return SourceLocation{source, begin};
    }

    [[nodiscard]]
    constexpr SourceLocation end_location() const noexcept
    {
        return SourceLocation{source, end};
    }

    auto operator<=>(const SourceSpan&) const = default;
};

} // namespace torel::source