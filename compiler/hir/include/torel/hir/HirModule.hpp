#pragma once

#include "torel/hir/HirNode.hpp"

#include <cstddef>
#include <string>
#include <deque>

namespace torel::hir {

class HirModule final {
public:
    [[nodiscard]]
    NodeId add_node(
        HirKind kind,
        source::SourceSpan span,
        std::string text = {});

    void set_root(NodeId root) noexcept;

    [[nodiscard]]
    NodeId root() const noexcept;

    [[nodiscard]]
    const HirNode* get(NodeId id) const noexcept;

    [[nodiscard]]
    HirNode* get(NodeId id) noexcept;

    [[nodiscard]]
    std::size_t node_count() const noexcept;

private:
    std::deque<HirNode> nodes_;
    NodeId root_{};
};

} // namespace torel::hir