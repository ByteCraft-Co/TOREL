#include "torel/parser/TokenCursor.hpp"

namespace torel::parser {

TokenCursor::TokenCursor(
    std::span<const lexer::Token> tokens) noexcept
    : tokens_(tokens)
{
}

std::size_t TokenCursor::position() const noexcept
{
    return position_;
}

bool TokenCursor::at_end() const noexcept
{
    return tokens_.empty() ||
           position_ >= tokens_.size() ||
           current().kind == lexer::TokenKind::EndOfFile;
}

bool TokenCursor::at(
    lexer::TokenKind kind) const noexcept
{
    return !tokens_.empty() &&
           position_ < tokens_.size() &&
           current().kind == kind;
}

const lexer::Token&
TokenCursor::current() const noexcept
{
    if (position_ < tokens_.size()) {
        return tokens_[position_];
    }

    return tokens_.back();
}

const lexer::Token&
TokenCursor::peek(
    std::size_t distance) const noexcept
{
    const std::size_t target =
        position_ + distance;

    if (target < tokens_.size()) {
        return tokens_[target];
    }

    return tokens_.back();
}

void TokenCursor::advance() noexcept
{
    if (position_ < tokens_.size()) {
        ++position_;
    }
}

} // namespace torel::parser