#pragma once

#include "torel/hir/HirModule.hpp"
#include "torel/parser/SyntaxTree.hpp"
#include "torel/source/SourceManager.hpp"

#include <optional>
#include <string>

namespace torel::hir {

class HirLowerer final {
public:
    explicit HirLowerer(
        const source::SourceManager& sources) noexcept;

    [[nodiscard]]
    HirModule lower(
        const parser::SyntaxTree& tree) const;

private:
    [[nodiscard]]
    std::optional<NodeId> lower_node(
        const parser::SyntaxNode& syntax,
        const parser::SyntaxTree& tree,
        HirModule& module) const;

    [[nodiscard]]
    source::SourceSpan span_for_range(
        const parser::SyntaxTree& tree,
        std::size_t begin,
        std::size_t end) const noexcept;

    [[nodiscard]]
    std::string source_text(
        source::SourceSpan span) const;

    const source::SourceManager& sources_;
};

} // namespace torel::hir