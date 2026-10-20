#pragma once

#include "torel/lexer/Token.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace torel::parser {

enum class SyntaxKind : std::uint8_t {
    Root,
    Scope,
    Token,
    Error
};

struct SyntaxNode final {
    SyntaxKind kind = SyntaxKind::Error;

    // Half-open token-index range: [token_begin, token_end)
    std::size_t token_begin = 0;
    std::size_t token_end = 0;

    std::vector<SyntaxNode> children;

    [[nodiscard]]
    std::size_t token_count() const noexcept
    {
        return token_end >= token_begin
            ? token_end - token_begin
            : 0;
    }
};

struct SyntaxTree final {
    std::vector<lexer::Token> tokens;
    SyntaxNode root;
};

} // namespace torel::parser