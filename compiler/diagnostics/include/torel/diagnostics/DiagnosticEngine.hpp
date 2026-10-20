#pragma once

#include "torel/diagnostics/Diagnostic.hpp"

#include <cstddef>
#include <vector>

namespace torel::diagnostics {

class DiagnosticEngine final {
public:
    void emit(Diagnostic diagnostic);

    [[nodiscard]]
    const std::vector<Diagnostic>&
    diagnostics() const noexcept;

    [[nodiscard]]
    std::size_t error_count() const noexcept;

    [[nodiscard]]
    std::size_t warning_count() const noexcept;

    [[nodiscard]]
    bool has_errors() const noexcept;

    void clear() noexcept;

private:
    std::vector<Diagnostic> diagnostics_;
    std::size_t error_count_ = 0;
    std::size_t warning_count_ = 0;
};

} // namespace torel::diagnostics