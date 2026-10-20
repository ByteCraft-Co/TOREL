#pragma once

#include <cstdint>

namespace torel::hir {

enum class HirKind : std::uint8_t {
    Root,
    Scope,

    Name,

    IntegerLiteral,
    FloatLiteral,
    StringLiteral,

    // Syntax preserved across the CST -> HIR boundary but not yet
    // assigned semantics by the current lowering layer.
    UnloweredSyntax,

    Error
};

} // namespace torel::hir