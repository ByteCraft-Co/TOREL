#pragma once

#include "torel/source/SourceId.hpp"

#include <compare>
#include <cstdint>

namespace torel::source {

using ByteOffset = std::uint64_t;

struct SourceLocation final {
    SourceId source{};
    ByteOffset offset = 0;

    [[nodiscard]]
    constexpr bool is_valid() const noexcept
    {
        return source.is_valid();
    }

    auto operator<=>(const SourceLocation&) const = default;
};

} // namespace torel::source