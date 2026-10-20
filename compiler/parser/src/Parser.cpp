#include "torel/parser/Parser.hpp"

#include "torel/diagnostics/Diagnostic.hpp"
#include "torel/diagnostics/DiagnosticCode.hpp"
#include "torel/diagnostics/DiagnosticLabel.hpp"
#include "torel/diagnostics/DiagnosticSeverity.hpp"
#include "torel/lexer/TokenKind.hpp"

#include <cstddef>
#include <utility>

namespace torel::parser {

Parser::Parser(
    std::vector<lexer::Token> tokens,
    diagnostics::DiagnosticEngine& diagnostics)
    : tokens_(std::move(tokens)),
      cursor_(tokens_),
      diagnostics_(diagnostics)
{
}

SyntaxTree Parser::parse()
{
    SyntaxNode root = parse_root();

    return SyntaxTree{
        std::move(tokens_),
        std::move(root)
    };
}

SyntaxNode Parser::parse_root()
{
    SyntaxNode root{
        SyntaxKind::Root,
        0,
        0,
        {}
    };

    while (!cursor_.at_end()) {
        if (cursor_.at(lexer::TokenKind::ScopeOpen)) {
            root.children.push_back(parse_scope());
            continue;
        }

        if (cursor_.at(lexer::TokenKind::ScopeClose)) {
            root.children.push_back(
                parse_unexpected_scope_close());

            continue;
        }

        root.children.push_back(parse_token_node());
    }

    // Preserve the synthetic EOF token as a CST leaf.
    if (
        !tokens_.empty() &&
        cursor_.at(lexer::TokenKind::EndOfFile))
    {
        root.children.push_back(parse_token_node());
    }

    root.token_end = cursor_.position();

    return root;
}

SyntaxNode Parser::parse_scope()
{
    const std::size_t begin =
        cursor_.position();

    const lexer::Token opening_token =
        cursor_.current();

    cursor_.advance();

    SyntaxNode scope{
        SyntaxKind::Scope,
        begin,
        begin,
        {}
    };

    while (!cursor_.at_end()) {
        if (cursor_.at(lexer::TokenKind::ScopeClose)) {
            cursor_.advance();
            scope.token_end = cursor_.position();
            return scope;
        }

        if (cursor_.at(lexer::TokenKind::ScopeOpen)) {
            scope.children.push_back(parse_scope());
            continue;
        }

        scope.children.push_back(parse_token_node());
    }

    report_unclosed_scope(opening_token);

    scope.token_end = cursor_.position();

    return scope;
}

SyntaxNode Parser::parse_token_node()
{
    const std::size_t begin =
        cursor_.position();

    cursor_.advance();

    return SyntaxNode{
        SyntaxKind::Token,
        begin,
        cursor_.position(),
        {}
    };
}

SyntaxNode Parser::parse_unexpected_scope_close()
{
    const std::size_t begin =
        cursor_.position();

    const lexer::Token token =
        cursor_.current();

    report_unexpected_scope_close(token);
    cursor_.advance();

    return SyntaxNode{
        SyntaxKind::Error,
        begin,
        cursor_.position(),
        {}
    };
}

void Parser::report_unexpected_scope_close(
    const lexer::Token& token)
{
    using namespace diagnostics;

    Diagnostic diagnostic{
        DiagnosticSeverity::Error,
        DiagnosticCode{"PAR0001"},
        "unexpected scope terminator",
        {},
        {}
    };

    diagnostic.labels.push_back(
        DiagnosticLabel{
            token.span,
            DiagnosticLabelKind::Primary,
            "this ':|' has no matching '|:'"
        });

    diagnostics_.emit(std::move(diagnostic));
}

void Parser::report_unclosed_scope(
    const lexer::Token& opening_token)
{
    using namespace diagnostics;

    Diagnostic diagnostic{
        DiagnosticSeverity::Error,
        DiagnosticCode{"PAR0002"},
        "unclosed scope",
        {},
        {}
    };

    diagnostic.labels.push_back(
        DiagnosticLabel{
            opening_token.span,
            DiagnosticLabelKind::Primary,
            "scope begins here"
        });

    diagnostic.notes.push_back(
        "expected ':|' before end of file");

    diagnostics_.emit(std::move(diagnostic));
}

} // namespace torel::parser