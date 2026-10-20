#include "torel/hir/HirLowerer.hpp"

#include "torel/hir/HirKind.hpp"
#include "torel/lexer/TokenKind.hpp"
#include "torel/parser/SyntaxTree.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <utility>

namespace torel::hir {

HirLowerer::HirLowerer(
    const source::SourceManager& sources) noexcept
    : sources_(sources)
{
}

HirModule HirLowerer::lower(
    const parser::SyntaxTree& tree) const
{
    HirModule module;

    const auto root =
        lower_node(
            tree.root,
            tree,
            module);

    if (root.has_value()) {
        module.set_root(*root);
    }

    return module;
}

std::optional<NodeId>
HirLowerer::lower_node(
    const parser::SyntaxNode& syntax,
    const parser::SyntaxTree& tree,
    HirModule& module) const
{
    using parser::SyntaxKind;

    if (syntax.kind == SyntaxKind::Token) {
        if (
            syntax.token_begin >= tree.tokens.size())
        {
            return module.add_node(
                HirKind::Error,
                {});
        }

        const lexer::Token& token =
            tree.tokens[syntax.token_begin];

        const auto span = token.span;

        switch (token.kind) {
        case lexer::TokenKind::EndOfFile:
            return std::nullopt;

        case lexer::TokenKind::Identifier:
            return module.add_node(
                HirKind::Name,
                span,
                source_text(span));

        case lexer::TokenKind::IntegerLiteral:
            return module.add_node(
                HirKind::IntegerLiteral,
                span,
                source_text(span));

        case lexer::TokenKind::FloatLiteral:
            return module.add_node(
                HirKind::FloatLiteral,
                span,
                source_text(span));

        case lexer::TokenKind::StringLiteral:
            return module.add_node(
                HirKind::StringLiteral,
                span,
                source_text(span));

        case lexer::TokenKind::Invalid:
            return module.add_node(
                HirKind::Error,
                span,
                source_text(span));

        default:
            return module.add_node(
                HirKind::UnloweredSyntax,
                span,
                source_text(span));
        }
    }

    const auto span =
        span_for_range(
            tree,
            syntax.token_begin,
            syntax.token_end);

    HirKind kind = HirKind::Error;

    switch (syntax.kind) {
    case SyntaxKind::Root:
        kind = HirKind::Root;
        break;

    case SyntaxKind::Scope:
        kind = HirKind::Scope;
        break;

    case SyntaxKind::Error:
        kind = HirKind::Error;
        break;

    case SyntaxKind::Token:
        // Handled above.
        break;
    }

    const NodeId id =
        module.add_node(
            kind,
            span);

    if (syntax.kind == SyntaxKind::Root) {
        module.set_root(id);
    }

    for (const auto& child : syntax.children) {
        const auto child_id =
            lower_node(
                child,
                tree,
                module);

        if (!child_id.has_value()) {
            continue;
        }

        HirNode* node =
            module.get(id);

        if (node != nullptr) {
            node->children.push_back(*child_id);
        }
    }

    return id;
}

source::SourceSpan
HirLowerer::span_for_range(
    const parser::SyntaxTree& tree,
    std::size_t begin,
    std::size_t end) const noexcept
{
    if (
        begin >= end ||
        begin >= tree.tokens.size() ||
        end > tree.tokens.size())
    {
        return {};
    }

    const lexer::Token& first =
        tree.tokens[begin];

    const lexer::Token& last =
        tree.tokens[end - 1];

    if (
        !first.span.source.is_valid() ||
        first.span.source != last.span.source)
    {
        return {};
    }

    return source::SourceSpan{
        first.span.source,
        first.span.begin,
        last.span.end
    };
}

std::string HirLowerer::source_text(
    source::SourceSpan span) const
{
    const auto text =
        sources_.text(span);

    if (!text.has_value()) {
        return {};
    }

    return std::string{*text};
}

} // namespace torel::hir