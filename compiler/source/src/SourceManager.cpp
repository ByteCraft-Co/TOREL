#include "torel/source/SourceManager.hpp"

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace torel::source {

SourceId SourceManager::add_source(
    std::filesystem::path path,
    std::string contents)
{
    if (
        files_.size() >=
        static_cast<std::size_t>(SourceId::invalid_value))
    {
        throw std::length_error{
            "TOREL SourceManager exhausted SourceId space"
        };
    }

    const auto id = SourceId{
        static_cast<SourceId::Value>(files_.size())
    };

    files_.push_back(
        std::make_unique<SourceFile>(
            id,
            std::move(path),
            std::move(contents)));

    return id;
}

const SourceFile*
SourceManager::get(SourceId id) const noexcept
{
    if (!id.is_valid()) {
        return nullptr;
    }

    const auto index =
        static_cast<std::size_t>(id.value());

    if (index >= files_.size()) {
        return nullptr;
    }

    return files_[index].get();
}

std::optional<LineColumn>
SourceManager::line_column(
    SourceLocation location) const noexcept
{
    const SourceFile* file = get(location.source);

    if (file == nullptr) {
        return std::nullopt;
    }

    return file->line_column(location.offset);
}

std::optional<std::string_view>
SourceManager::text(SourceSpan span) const noexcept
{
    if (!span.is_valid()) {
        return std::nullopt;
    }

    const SourceFile* file = get(span.source);

    if (file == nullptr) {
        return std::nullopt;
    }

    return file->slice(span.begin, span.end);
}

std::size_t SourceManager::file_count() const noexcept
{
    return files_.size();
}

} // namespace torel::source