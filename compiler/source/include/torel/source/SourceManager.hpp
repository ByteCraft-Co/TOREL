#pragma once

#include "torel/source/SourceFile.hpp"
#include "torel/source/SourceSpan.hpp"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace torel::source {

class SourceManager final {
public:
    [[nodiscard]]
    SourceId add_source(
        std::filesystem::path path,
        std::string contents);

    [[nodiscard]]
    const SourceFile* get(SourceId id) const noexcept;

    [[nodiscard]]
    std::optional<LineColumn>
    line_column(SourceLocation location) const noexcept;

    [[nodiscard]]
    std::optional<std::string_view>
    text(SourceSpan span) const noexcept;

    [[nodiscard]]
    std::size_t file_count() const noexcept;

private:
    std::vector<std::unique_ptr<SourceFile>> files_;
};

} // namespace torel::source