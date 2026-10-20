#pragma once

#include "torel/source/SourceLocation.hpp"

#include <compare>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace torel::source {

struct LineColumn final {
    std::uint64_t line = 1;
    std::uint64_t byte_column = 1;

    auto operator<=>(const LineColumn&) const = default;
};

class SourceFile final {
public:
    SourceFile(
        SourceId id,
        std::filesystem::path path,
        std::string contents);

    [[nodiscard]]
    SourceId id() const noexcept;

    [[nodiscard]]
    const std::filesystem::path& path() const noexcept;

    [[nodiscard]]
    std::string_view contents() const noexcept;

    [[nodiscard]]
    ByteOffset size_bytes() const noexcept;

    [[nodiscard]]
    std::size_t line_count() const noexcept;

    [[nodiscard]]
    std::optional<LineColumn>
    line_column(ByteOffset offset) const noexcept;

    [[nodiscard]]
    std::optional<std::string_view>
    slice(ByteOffset begin, ByteOffset end) const noexcept;

private:
    void build_line_index();

    SourceId id_;
    std::filesystem::path path_;
    std::string contents_;
    std::vector<ByteOffset> line_starts_;
};

} // namespace torel::source