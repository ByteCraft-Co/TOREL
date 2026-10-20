#pragma once

#include "torel/source/SourceSpan.hpp"

#include <cstdint>
#include <string>

namespace torel::diagnostics {

enum class DiagnosticLabelKind : std::uint8_t {
    Primary,
    Secondary
};

struct DiagnosticLabel final {
    source::SourceSpan span{};
    DiagnosticLabelKind kind =
        DiagnosticLabelKind::Secondary;
    std::string message;
};

} // namespace torel::diagnostics