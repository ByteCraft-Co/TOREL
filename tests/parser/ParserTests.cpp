#include "torel/diagnostics/DiagnosticEngine.hpp"
#include "torel/lexer/Lexer.hpp"
#include "torel/lexer/TokenKind.hpp"
#include "torel/parser/Parser.hpp"
#include "torel/parser/SyntaxTree.hpp"
#include "torel/source/SourceManager.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace {

void expect(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error{std::string{message}};
    }
}

torel::parser::SyntaxTree parse_source(
    std::string text,
    torel::source::SourceManager& sources,
    torel::diagnostics::DiagnosticEngine& diagnostics)
{
    const auto source_id =
        sources.add_source(
            "parser-test.torel",
            std::move(text));

    const auto* source_file =
        sources.get(source_id);

    expect(
        source_file != nullptr,
        "parser test source must resolve");

    torel::lexer::Lexer lexer{
        *source_file,
        diagnostics
    };

    auto tokens = lexer.lex_all();

    torel::parser::Parser parser{
        std::move(tokens),
        diagnostics
    };

    return parser.parse();
}

void test_flat_token_preservation()
{
    using namespace torel::parser;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto tree =
        parse_source(
            "alpha :: 12",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 0,
        "flat source must parse without errors");

    expect(
        tree.root.kind == SyntaxKind::Root,
        "root node kind mismatch");

    expect(
        tree.root.children.size() == 4,
        "root must preserve three tokens plus EOF");

    expect(
        tree.tokens.size() == 4,
        "syntax tree must preserve all lexer tokens");

    for (const auto& child : tree.root.children) {
        expect(
            child.kind == SyntaxKind::Token,
            "flat source children must be token leaves");
    }
}

void test_explicit_scope()
{
    using namespace torel::parser;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto tree =
        parse_source(
            "|: alpha :|",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 0,
        "valid scope must parse without errors");

    expect(
        tree.root.children.size() == 2,
        "root must contain scope and EOF");

    const auto& scope =
        tree.root.children[0];

    expect(
        scope.kind == SyntaxKind::Scope,
        "first root child must be scope");

    expect(
        scope.children.size() == 1,
        "scope must contain alpha token");

    expect(
        scope.children[0].kind == SyntaxKind::Token,
        "scope content must remain token leaf");

    expect(
        scope.token_count() == 3,
        "scope range must include opener, body, and closer");
}

void test_nested_scopes()
{
    using namespace torel::parser;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto tree =
        parse_source(
            "|: outer |: inner :| tail :|",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 0,
        "nested scopes must parse without errors");

    const auto& outer =
        tree.root.children[0];

    expect(
        outer.kind == SyntaxKind::Scope,
        "outer node must be scope");

    expect(
        outer.children.size() == 3,
        "outer scope must contain token, nested scope, token");

    expect(
        outer.children[1].kind == SyntaxKind::Scope,
        "middle outer child must be nested scope");

    expect(
        outer.children[1].children.size() == 1,
        "nested scope must contain one token");
}

void test_unexpected_scope_close_recovery()
{
    using namespace torel::parser;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto tree =
        parse_source(
            "alpha :| beta",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 1,
        "unmatched scope close must emit one error");

    expect(
        diagnostics.diagnostics()[0].code.value() ==
            "PAR0001",
        "unexpected scope close code mismatch");

    expect(
        tree.root.children.size() == 4,
        "parser must recover and preserve following tokens");

    expect(
        tree.root.children[1].kind == SyntaxKind::Error,
        "unmatched close must become error CST node");

    expect(
        tree.root.children[2].kind == SyntaxKind::Token,
        "parser must continue after unmatched close");
}

void test_unclosed_scope_recovery()
{
    using namespace torel::parser;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto tree =
        parse_source(
            "|: alpha beta",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 1,
        "unclosed scope must emit one error");

    expect(
        diagnostics.diagnostics()[0].code.value() ==
            "PAR0002",
        "unclosed scope code mismatch");

    expect(
        tree.root.children.size() == 2,
        "root must preserve recovered scope and EOF");

    expect(
        tree.root.children[0].kind == SyntaxKind::Scope,
        "recovered node must remain a scope");

    expect(
        tree.root.children[0].children.size() == 2,
        "unclosed scope must preserve its body");
}

void test_token_ranges_are_half_open()
{
    using namespace torel::parser;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto tree =
        parse_source(
            "|: x :|",
            sources,
            diagnostics);

    const auto& scope =
        tree.root.children[0];

    expect(
        scope.token_begin == 0,
        "scope token begin mismatch");

    expect(
        scope.token_end == 3,
        "scope token end must be one past closing token");

    expect(
        scope.token_count() == 3,
        "scope half-open token range mismatch");
}

} // namespace

int main()
{
    try {
        test_flat_token_preservation();
        test_explicit_scope();
        test_nested_scopes();
        test_unexpected_scope_close_recovery();
        test_unclosed_scope_recovery();
        test_token_ranges_are_half_open();
    }
    catch (const std::exception& exception) {
        std::cerr
            << "parser test failure: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }

    std::cout
        << "TOREL parser foundation tests passed\n";

    return EXIT_SUCCESS;
}