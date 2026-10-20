#pragma once

#include "torel/lexer/Token.hpp"
#include "torel/lexer/TokenKind.hpp"

#include <cstddef>
#include <span>

namespace torel::parser {

class TokenCursor final {
public:
    explicit TokenCursor(
        std::span<const lexer::Token> tokens) noexcept;

    [[nodiscard]]
    std::size_t position() const noexcept;

    [[nodiscard]]
    bool at_end() const noexcept;

    [[nodiscard]]
    bool at(lexer::TokenKind kind) const noexcept;

    [[nodiscard]]
    const lexer::Token& current() const noexcept;

    [[nodiscard]]
    const lexer::Token& peek(
        std::size_t distance = 1) const noexcept;

    void advance() noexcept;

private:
    std::span<const lexer::Token> tokens_;
    std::size_t position_ = 0;
};

} // namespace torel::parser