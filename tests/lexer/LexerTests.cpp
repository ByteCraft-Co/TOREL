#include "torel/diagnostics/DiagnosticEngine.hpp"
#include "torel/lexer/Lexer.hpp"
#include "torel/lexer/TokenKind.hpp"
#include "torel/source/SourceManager.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

void expect(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error{std::string{message}};
    }
}

std::vector<torel::lexer::Token>
lex(
    std::string source_text,
    torel::source::SourceManager& sources,
    torel::diagnostics::DiagnosticEngine& diagnostics)
{
    const auto id =
        sources.add_source(
            "test.torel",
            std::move(source_text));

    const auto* file = sources.get(id);

    expect(
        file != nullptr,
        "test source must resolve");

    torel::lexer::Lexer lexer{
        *file,
        diagnostics
    };

    return lexer.lex_all();
}

void test_basic_establishment()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            "energy # i64 :: 600",
            sources,
            diagnostics);

    expect(tokens.size() == 6, "unexpected token count");

    expect(
        tokens[0].kind == TokenKind::Identifier,
        "energy must be identifier");

    expect(
        tokens[1].kind == TokenKind::Hash,
        "# must be Hash");

    expect(
        tokens[2].kind == TokenKind::Identifier,
        "i64 must be identifier");

    expect(
        tokens[3].kind == TokenKind::Establish,
        ":: must be Establish");

    expect(
        tokens[4].kind == TokenKind::IntegerLiteral,
        "600 must be integer literal");

    expect(
        tokens[5].kind == TokenKind::EndOfFile,
        "final token must be EOF");

    expect(
        !diagnostics.has_errors(),
        "valid source must not produce errors");
}

void test_longest_match()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            ":?& :? :/? :/",
            sources,
            diagnostics);

    expect(tokens.size() == 5, "longest-match token count mismatch");

    expect(
        tokens[0].kind == TokenKind::RuntimeRequire,
        ":?& must be one token");

    expect(
        tokens[1].kind == TokenKind::QuestionBranch,
        ":? must be one token");

    expect(
        tokens[2].kind == TokenKind::CancelCheckpoint,
        ":/? must be one token");

    expect(
        tokens[3].kind == TokenKind::Cancel,
        ":/ must be one token");

    expect(
        tokens[4].kind == TokenKind::EndOfFile,
        "final token must be EOF");
}

void test_structural_tokens()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            "|: :| <| || |> [| |] [: :] [. .] [? ?]",
            sources,
            diagnostics);

    const TokenKind expected[] = {
        TokenKind::ScopeOpen,
        TokenKind::ScopeClose,
        TokenKind::PortOpen,
        TokenKind::PortSeparator,
        TokenKind::PortClose,
        TokenKind::ShapeOpen,
        TokenKind::ShapeClose,
        TokenKind::RealizationOpen,
        TokenKind::RealizationClose,
        TokenKind::GroupOpen,
        TokenKind::GroupClose,
        TokenKind::ExposureOpen,
        TokenKind::ExposureClose,
        TokenKind::EndOfFile
    };

    expect(
        tokens.size() ==
            (sizeof(expected) / sizeof(expected[0])),
        "structural token count mismatch");

    for (std::size_t index = 0;
         index < tokens.size();
         ++index)
    {
        expect(
            tokens[index].kind == expected[index],
            "structural token kind mismatch");
    }

    expect(
        !diagnostics.has_errors(),
        "structural tokens must lex without errors");
}

void test_literals()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            "42 84.6 \"Nova\"",
            sources,
            diagnostics);

    expect(
        tokens[0].kind == TokenKind::IntegerLiteral,
        "42 must be integer literal");

    expect(
        tokens[1].kind == TokenKind::FloatLiteral,
        "84.6 must be float literal");

    expect(
        tokens[2].kind == TokenKind::StringLiteral,
        "Nova must be string literal");

    expect(
        !diagnostics.has_errors(),
        "valid literals must not produce errors");
}

void test_exact_spans()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            "alpha :: 12",
            sources,
            diagnostics);

    expect(tokens[0].span.begin == 0, "alpha span begin mismatch");
    expect(tokens[0].span.end == 5, "alpha span end mismatch");

    expect(tokens[1].span.begin == 6, ":: span begin mismatch");
    expect(tokens[1].span.end == 8, ":: span end mismatch");

    expect(tokens[2].span.begin == 9, "12 span begin mismatch");
    expect(tokens[2].span.end == 11, "12 span end mismatch");
}

void test_unexpected_byte_diagnostic()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            "alpha = 1",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 1,
        "unexpected byte must emit one error");

    expect(
        diagnostics.diagnostics()[0].code.value() ==
            "LEX0001",
        "unexpected byte diagnostic code mismatch");

    bool found_invalid = false;

    for (const auto& token : tokens) {
        if (token.kind == TokenKind::Invalid) {
            found_invalid = true;
        }
    }

    expect(
        found_invalid,
        "unexpected byte must produce Invalid token");
}

void test_unterminated_string()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            "\"unfinished",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 1,
        "unterminated string must emit one error");

    expect(
        diagnostics.diagnostics()[0].code.value() ==
            "LEX0002",
        "unterminated string code mismatch");

    expect(
        tokens[0].kind == TokenKind::Invalid,
        "unterminated string must produce Invalid token");
}

void test_capability_and_output_glyphs()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            "item # &? Drawable ^ # i64",
            sources,
            diagnostics);

    const TokenKind expected[] = {
        TokenKind::Identifier,
        TokenKind::Hash,
        TokenKind::Ampersand,
        TokenKind::Question,
        TokenKind::Identifier,
        TokenKind::Caret,
        TokenKind::Hash,
        TokenKind::Identifier,
        TokenKind::EndOfFile
    };

    expect(
        tokens.size() ==
            (sizeof(expected) / sizeof(expected[0])),
        "capability/output token count mismatch");

    for (std::size_t index = 0;
         index < tokens.size();
         ++index)
    {
        expect(
            tokens[index].kind == expected[index],
            "capability/output token kind mismatch");
    }

    expect(
        !diagnostics.has_errors(),
        "planned capability/output glyphs must lex cleanly");
}
void test_whitespace_is_trivia()
{
    using namespace torel::lexer;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    const auto tokens =
        lex(
            "alpha\t\r\n    beta",
            sources,
            diagnostics);

    expect(
        tokens.size() == 3,
        "whitespace must not produce tokens");

    expect(
        tokens[0].kind == TokenKind::Identifier,
        "alpha must remain identifier");

    expect(
        tokens[1].kind == TokenKind::Identifier,
        "beta must remain identifier");

    expect(
        tokens[2].kind == TokenKind::EndOfFile,
        "EOF token missing");
}

} // namespace

int main()
{
    try {
        test_basic_establishment();
        test_longest_match();
        test_structural_tokens();
        test_literals();
        test_exact_spans();
        test_unexpected_byte_diagnostic();
        test_unterminated_string();
        test_capability_and_output_glyphs();
        test_whitespace_is_trivia();
    }
    catch (const std::exception& exception) {
        std::cerr
            << "lexer test failure: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }

    std::cout
        << "TOREL lexer foundation tests passed\n";

    return EXIT_SUCCESS;
}