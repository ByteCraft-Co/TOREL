#pragma once

#include "torel/lexer/TokenKind.hpp"
#include "torel/source/SourceSpan.hpp"

namespace torel::lexer {

struct Token final {
    TokenKind kind = TokenKind::Invalid;
    source::SourceSpan span{};
};

} // namespace torel::lexer