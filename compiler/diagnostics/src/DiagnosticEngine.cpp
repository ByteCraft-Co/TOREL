#include "torel/diagnostics/DiagnosticEngine.hpp"

#include "torel/diagnostics/DiagnosticSeverity.hpp"

#include <utility>

namespace torel::diagnostics {

void DiagnosticEngine::emit(Diagnostic diagnostic)
{
    switch (diagnostic.severity) {
    case DiagnosticSeverity::Note:
        break;

    case DiagnosticSeverity::Warning:
        ++warning_count_;
        break;

    case DiagnosticSeverity::Error:
        ++error_count_;
        break;
    }

    diagnostics_.push_back(std::move(diagnostic));
}

const std::vector<Diagnostic>&
DiagnosticEngine::diagnostics() const noexcept
{
    return diagnostics_;
}

std::size_t
DiagnosticEngine::error_count() const noexcept
{
    return error_count_;
}

std::size_t
DiagnosticEngine::warning_count() const noexcept
{
    return warning_count_;
}

bool DiagnosticEngine::has_errors() const noexcept
{
    return error_count_ != 0;
}

void DiagnosticEngine::clear() noexcept
{
    diagnostics_.clear();
    error_count_ = 0;
    warning_count_ = 0;
}

} // namespace torel::diagnostics