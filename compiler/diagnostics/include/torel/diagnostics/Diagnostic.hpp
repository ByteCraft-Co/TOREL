#pragma once

#include "torel/diagnostics/DiagnosticCode.hpp"
#include "torel/diagnostics/DiagnosticLabel.hpp"
#include "torel/diagnostics/DiagnosticSeverity.hpp"

#include <string>
#include <vector>

namespace torel::diagnostics {

struct Diagnostic final {
    DiagnosticSeverity severity;
    DiagnosticCode code;
    std::string message;

    std::vector<DiagnosticLabel> labels;
    std::vector<std::string> notes;
};

} // namespace torel::diagnostics