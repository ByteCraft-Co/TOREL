#pragma once

#include <cstdint>

namespace torel::diagnostics {

enum class DiagnosticSeverity : std::uint8_t {
    Note,
    Warning,
    Error
};

} // namespace torel::diagnostics