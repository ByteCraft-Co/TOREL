#pragma once

#include "torel/diagnostics/DiagnosticEngine.hpp"
#include "torel/lexer/Token.hpp"
#include "torel/parser/SyntaxTree.hpp"
#include "torel/parser/TokenCursor.hpp"

#include <vector>

namespace torel::parser {

class Parser final {
public:
    Parser(
        std::vector<lexer::Token> tokens,
        diagnostics::DiagnosticEngine& diagnostics);

    [[nodiscard]]
    SyntaxTree parse();

private:
    [[nodiscard]]
    SyntaxNode parse_root();

    [[nodiscard]]
    SyntaxNode parse_scope();

    [[nodiscard]]
    SyntaxNode parse_token_node();

    [[nodiscard]]
    SyntaxNode parse_unexpected_scope_close();

    void report_unexpected_scope_close(
        const lexer::Token& token);

    void report_unclosed_scope(
        const lexer::Token& opening_token);

    std::vector<lexer::Token> tokens_;
    TokenCursor cursor_;
    diagnostics::DiagnosticEngine& diagnostics_;
};

} // namespace torel::parser