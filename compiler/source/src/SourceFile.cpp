#include "torel/source/SourceFile.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>

namespace torel::source {

SourceFile::SourceFile(
    SourceId id,
    std::filesystem::path path,
    std::string contents)
    : id_(id),
      path_(std::move(path)),
      contents_(std::move(contents))
{
    build_line_index();
}

SourceId SourceFile::id() const noexcept
{
    return id_;
}

const std::filesystem::path&
SourceFile::path() const noexcept
{
    return path_;
}

std::string_view
SourceFile::contents() const noexcept
{
    return contents_;
}

ByteOffset
SourceFile::size_bytes() const noexcept
{
    return static_cast<ByteOffset>(contents_.size());
}

std::size_t
SourceFile::line_count() const noexcept
{
    return line_starts_.size();
}

std::optional<LineColumn>
SourceFile::line_column(ByteOffset offset) const noexcept
{
    if (offset > size_bytes()) {
        return std::nullopt;
    }

    const auto upper =
        std::upper_bound(
            line_starts_.begin(),
            line_starts_.end(),
            offset);

    auto line_start_iterator = upper;

    if (line_start_iterator != line_starts_.begin()) {
        --line_start_iterator;
    }

    const auto line_index =
        static_cast<std::uint64_t>(
            std::distance(
                line_starts_.begin(),
                line_start_iterator));

    const ByteOffset line_start = *line_start_iterator;

    return LineColumn{
        line_index + 1,
        offset - line_start + 1
    };
}

std::optional<std::string_view>
SourceFile::slice(
    ByteOffset begin,
    ByteOffset end) const noexcept
{
    if (begin > end || end > size_bytes()) {
        return std::nullopt;
    }

    const auto begin_index =
        static_cast<std::size_t>(begin);

    const auto length =
        static_cast<std::size_t>(end - begin);

    return std::string_view{contents_}.substr(
        begin_index,
        length);
}

void SourceFile::build_line_index()
{
    line_starts_.clear();
    line_starts_.push_back(0);

    std::size_t index = 0;

    while (index < contents_.size()) {
        const char current = contents_[index];

        if (current == '\r') {
            ++index;

            if (
                index < contents_.size() &&
                contents_[index] == '\n')
            {
                ++index;
            }

            line_starts_.push_back(
                static_cast<ByteOffset>(index));

            continue;
        }

        if (current == '\n') {
            ++index;

            line_starts_.push_back(
                static_cast<ByteOffset>(index));

            continue;
        }

        ++index;
    }
}

} // namespace torel::source