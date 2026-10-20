#pragma once

#include "torel/hir/HirKind.hpp"
#include "torel/hir/NodeId.hpp"
#include "torel/source/SourceSpan.hpp"

#include <string>
#include <vector>

namespace torel::hir {

struct HirNode final {
    NodeId id{};
    HirKind kind = HirKind::Error;
    source::SourceSpan span{};

    // Raw source spelling when the node carries textual data.
    //
    // Examples:
    //   Name           -> player
    //   IntegerLiteral -> 600
    //   StringLiteral  -> "Nova"
    //
    // Literal decoding is intentionally not performed here yet.
    std::string text;

    std::vector<NodeId> children;
};

} // namespace torel::hir