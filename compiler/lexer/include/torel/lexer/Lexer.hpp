#pragma once

#include "torel/diagnostics/DiagnosticEngine.hpp"
#include "torel/lexer/Token.hpp"
#include "torel/source/SourceFile.hpp"

#include <cstddef>
#include <string_view>
#include <vector>

namespace torel::lexer {

class Lexer final {
public:
    Lexer(
        const source::SourceFile& source_file,
        diagnostics::DiagnosticEngine& diagnostics);

    [[nodiscard]]
    std::vector<Token> lex_all();

private:
    [[nodiscard]]
    bool at_end() const noexcept;

    [[nodiscard]]
    char current() const noexcept;

    [[nodiscard]]
    char peek(std::size_t distance = 1) const noexcept;

    void advance(std::size_t count = 1) noexcept;

    void skip_whitespace() noexcept;

    [[nodiscard]]
    Token lex_identifier();

    [[nodiscard]]
    Token lex_number();

    [[nodiscard]]
    Token lex_string();

    [[nodiscard]]
    Token lex_punctuation();

    [[nodiscard]]
    Token make_token(
        TokenKind kind,
        std::size_t begin,
        std::size_t end) const noexcept;

    void report_unexpected_byte(std::size_t offset);

    void report_unterminated_string(
        std::size_t begin,
        std::size_t end);

    [[nodiscard]]
    bool matches(std::string_view text) const noexcept;

    const source::SourceFile& source_file_;
    diagnostics::DiagnosticEngine& diagnostics_;
    std::string_view source_;
    std::size_t offset_ = 0;
};

} // namespace torel::lexer