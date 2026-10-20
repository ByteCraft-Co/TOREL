#include "torel/hir/HirModule.hpp"

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>

namespace torel::hir {

NodeId HirModule::add_node(
    HirKind kind,
    source::SourceSpan span,
    std::string text)
{
    if (
        nodes_.size() >=
        static_cast<std::size_t>(NodeId::invalid_value))
    {
        throw std::length_error{
            "TOREL HIR exhausted NodeId space"
        };
    }

    const NodeId id{
        static_cast<NodeId::Value>(nodes_.size())
    };

    nodes_.push_back(
        HirNode{
            id,
            kind,
            span,
            std::move(text),
            {}
        });

    return id;
}

void HirModule::set_root(NodeId root) noexcept
{
    root_ = root;
}

NodeId HirModule::root() const noexcept
{
    return root_;
}

const HirNode*
HirModule::get(NodeId id) const noexcept
{
    if (!id.is_valid()) {
        return nullptr;
    }

    const auto index =
        static_cast<std::size_t>(id.value());

    if (index >= nodes_.size()) {
        return nullptr;
    }

    return &nodes_[index];
}

HirNode*
HirModule::get(NodeId id) noexcept
{
    if (!id.is_valid()) {
        return nullptr;
    }

    const auto index =
        static_cast<std::size_t>(id.value());

    if (index >= nodes_.size()) {
        return nullptr;
    }

    return &nodes_[index];
}

std::size_t HirModule::node_count() const noexcept
{
    return nodes_.size();
}

} // namespace torel::hir