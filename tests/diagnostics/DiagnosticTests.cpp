#include "torel/diagnostics/Diagnostic.hpp"
#include "torel/diagnostics/DiagnosticEngine.hpp"
#include "torel/diagnostics/DiagnosticLabel.hpp"
#include "torel/diagnostics/DiagnosticSeverity.hpp"
#include "torel/source/SourceManager.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace {

void expect(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error{std::string{message}};
    }
}

void test_error_diagnostic()
{
    using namespace torel::diagnostics;
    using namespace torel::source;

    SourceManager sources;
    DiagnosticEngine diagnostics;

    const SourceId source_id =
        sources.add_source(
            "sample.torel",
            "energy # i64 :: 600");

    Diagnostic diagnostic{
        DiagnosticSeverity::Error,
        DiagnosticCode{"E0001"},
        "example compiler error",
        {},
        {}
    };

    diagnostic.labels.push_back(
        DiagnosticLabel{
            SourceSpan{source_id, 0, 6},
            DiagnosticLabelKind::Primary,
            "primary source region"
        });

    diagnostic.labels.push_back(
        DiagnosticLabel{
            SourceSpan{source_id, 9, 12},
            DiagnosticLabelKind::Secondary,
            "related source region"
        });

    diagnostic.notes.push_back(
        "additional structured information");

    diagnostics.emit(std::move(diagnostic));

    expect(
        diagnostics.diagnostics().size() == 1,
        "diagnostic engine must contain one diagnostic");

    expect(
        diagnostics.error_count() == 1,
        "error count must be one");

    expect(
        diagnostics.warning_count() == 0,
        "warning count must remain zero");

    expect(
        diagnostics.has_errors(),
        "engine must report that errors exist");

    const Diagnostic& stored =
        diagnostics.diagnostics().front();

    expect(
        stored.code.value() == "E0001",
        "diagnostic code mismatch");

    expect(
        stored.message == "example compiler error",
        "diagnostic message mismatch");

    expect(
        stored.labels.size() == 2,
        "multi-label diagnostic must preserve both labels");

    expect(
        stored.labels[0].kind ==
            DiagnosticLabelKind::Primary,
        "first label must remain primary");

    expect(
        stored.labels[1].kind ==
            DiagnosticLabelKind::Secondary,
        "second label must remain secondary");

    expect(
        stored.labels[0].span.source == source_id,
        "primary label must preserve source identity");

    expect(
        stored.notes.size() == 1,
        "diagnostic note must be preserved");

    const auto primary_text =
        sources.text(stored.labels[0].span);

    expect(
        primary_text.has_value(),
        "primary label span must resolve");

    expect(
        *primary_text == "energy",
        "primary label must identify exact source bytes");
}

void test_warning_diagnostic()
{
    using namespace torel::diagnostics;

    DiagnosticEngine diagnostics;

    diagnostics.emit(
        Diagnostic{
            DiagnosticSeverity::Warning,
            DiagnosticCode{"W0001"},
            "example warning",
            {},
            {}
        });

    expect(
        diagnostics.warning_count() == 1,
        "warning count must be one");

    expect(
        diagnostics.error_count() == 0,
        "warning must not increment error count");

    expect(
        !diagnostics.has_errors(),
        "warning-only engine must not report errors");
}

void test_note_diagnostic()
{
    using namespace torel::diagnostics;

    DiagnosticEngine diagnostics;

    diagnostics.emit(
        Diagnostic{
            DiagnosticSeverity::Note,
            DiagnosticCode{"N0001"},
            "example note",
            {},
            {}
        });

    expect(
        diagnostics.diagnostics().size() == 1,
        "note must be retained");

    expect(
        diagnostics.error_count() == 0,
        "note must not increment error count");

    expect(
        diagnostics.warning_count() == 0,
        "note must not increment warning count");
}

void test_empty_diagnostic_code_rejected()
{
    using namespace torel::diagnostics;

    bool rejected = false;

    try {
        [[maybe_unused]]
        DiagnosticCode code{""};
    }
    catch (const std::invalid_argument&) {
        rejected = true;
    }

    expect(
        rejected,
        "empty diagnostic code must be rejected");
}
void test_clear()
{
    using namespace torel::diagnostics;

    DiagnosticEngine diagnostics;

    diagnostics.emit(
        Diagnostic{
            DiagnosticSeverity::Error,
            DiagnosticCode{"E0002"},
            "first error",
            {},
            {}
        });

    diagnostics.emit(
        Diagnostic{
            DiagnosticSeverity::Warning,
            DiagnosticCode{"W0002"},
            "first warning",
            {},
            {}
        });

    expect(
        diagnostics.diagnostics().size() == 2,
        "engine must contain two diagnostics");

    diagnostics.clear();

    expect(
        diagnostics.diagnostics().empty(),
        "clear must remove all diagnostics");

    expect(
        diagnostics.error_count() == 0,
        "clear must reset error count");

    expect(
        diagnostics.warning_count() == 0,
        "clear must reset warning count");

    expect(
        !diagnostics.has_errors(),
        "cleared engine must report no errors");
}

} // namespace

int main()
{
    try {
        test_error_diagnostic();
        test_warning_diagnostic();
        test_note_diagnostic();
        test_empty_diagnostic_code_rejected();
        test_clear();
    }
    catch (const std::exception& exception) {
        std::cerr
            << "diagnostics test failure: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }

    std::cout
        << "TOREL diagnostics foundation tests passed\n";

    return EXIT_SUCCESS;
}
