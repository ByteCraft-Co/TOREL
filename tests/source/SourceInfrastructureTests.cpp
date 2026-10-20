#include "torel/source/SourceFile.hpp"
#include "torel/source/SourceId.hpp"
#include "torel/source/SourceLocation.hpp"
#include "torel/source/SourceManager.hpp"
#include "torel/source/SourceSpan.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

void expect(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error{std::string{message}};
    }
}

void test_source_id()
{
    using torel::source::SourceId;

    const SourceId invalid{};
    expect(!invalid.is_valid(), "default SourceId must be invalid");

    const SourceId valid{7};
    expect(valid.is_valid(), "explicit SourceId must be valid");
    expect(valid.value() == 7, "SourceId value mismatch");
}

void test_source_manager_identity()
{
    using namespace torel::source;

    SourceManager manager;

    const SourceId first =
        manager.add_source("first.torel", "alpha");

    const SourceId second =
        manager.add_source("second.torel", "beta");

    expect(first.value() == 0, "first SourceId must be zero");
    expect(second.value() == 1, "second SourceId must be one");
    expect(first != second, "SourceIds must be unique");
    expect(manager.file_count() == 2, "file count mismatch");

    expect(
        manager.get(SourceId::invalid()) == nullptr,
        "invalid SourceId must not resolve");

    expect(
        manager.get(SourceId{999}) == nullptr,
        "unknown SourceId must not resolve");
}

void test_exact_source_preservation()
{
    using namespace torel::source;

    SourceManager manager;

    const std::string original =
        "alpha\r\nbeta\nomega\rlast";

    const SourceId id =
        manager.add_source("mixed-lines.torel", original);

    const SourceFile* file = manager.get(id);

    expect(file != nullptr, "source file must resolve");
    expect(
        file->contents() == original,
        "source contents must be preserved exactly");

    expect(
        file->size_bytes() == original.size(),
        "source byte size mismatch");

    expect(
        file->line_count() == 4,
        "mixed line endings must produce four lines");
}

void test_line_mapping()
{
    using namespace torel::source;

    SourceManager manager;

    const SourceId id =
        manager.add_source(
            "locations.torel",
            "alpha\r\nbeta\nomega\rlast");

    const auto line1 =
        manager.line_column(SourceLocation{id, 0});

    expect(line1.has_value(), "offset 0 must resolve");
    expect(line1->line == 1, "offset 0 must be line 1");
    expect(
        line1->byte_column == 1,
        "offset 0 must be byte column 1");

    const auto line2 =
        manager.line_column(SourceLocation{id, 7});

    expect(line2.has_value(), "offset 7 must resolve");
    expect(line2->line == 2, "offset 7 must be line 2");
    expect(
        line2->byte_column == 1,
        "offset 7 must start line 2");

    const auto line3 =
        manager.line_column(SourceLocation{id, 12});

    expect(line3.has_value(), "offset 12 must resolve");
    expect(line3->line == 3, "offset 12 must be line 3");
    expect(
        line3->byte_column == 1,
        "offset 12 must start line 3");

    const auto line4 =
        manager.line_column(SourceLocation{id, 18});

    expect(line4.has_value(), "offset 18 must resolve");
    expect(line4->line == 4, "offset 18 must be line 4");
    expect(
        line4->byte_column == 1,
        "offset 18 must start line 4");

    const auto end =
        manager.line_column(SourceLocation{id, 22});

    expect(end.has_value(), "one-past-final-byte must resolve");
    expect(end->line == 4, "EOF must remain on line 4");
    expect(
        end->byte_column == 5,
        "EOF byte column mismatch");

    expect(
        !manager.line_column(SourceLocation{id, 23}).has_value(),
        "offset beyond EOF must fail");
}

void test_spans()
{
    using namespace torel::source;

    SourceManager manager;

    const SourceId id =
        manager.add_source(
            "span.torel",
            "alpha\r\nbeta\nomega");

    const SourceSpan beta{id, 7, 11};

    expect(beta.is_valid(), "beta span must be valid");
    expect(!beta.empty(), "beta span must not be empty");
    expect(beta.size_bytes() == 4, "beta span length mismatch");

    const auto text = manager.text(beta);

    expect(text.has_value(), "beta span must resolve");
    expect(*text == "beta", "beta span contents mismatch");

    const SourceSpan newline{id, 5, 7};

    const auto newline_text = manager.text(newline);

    expect(
        newline_text.has_value(),
        "CRLF span must resolve");

    expect(
        *newline_text == "\r\n",
        "source slicing must preserve CRLF bytes");

    const SourceSpan empty{id, 7, 7};

    expect(empty.is_valid(), "empty span must be valid");
    expect(empty.empty(), "empty span must report empty");
    expect(empty.size_bytes() == 0, "empty span size must be zero");

    const SourceSpan reversed{id, 11, 7};

    expect(
        !reversed.is_valid(),
        "reversed span must be invalid");

    expect(
        reversed.size_bytes() == 0,
        "invalid reversed span must not underflow");

    expect(
        !manager.text(reversed).has_value(),
        "invalid span must not resolve");

    const SourceSpan outside{id, 0, 999};

    expect(
        !manager.text(outside).has_value(),
        "out-of-range span must not resolve");
}

void test_utf8_byte_semantics()
{
    using namespace torel::source;

    SourceManager manager;

    // UTF-8 bytes:
    // A      = 1 byte
    // U+00E9 = 2 bytes: C3 A9
    // newline = 1 byte
    // B      = 1 byte
    const std::string source =
        "A\xC3\xA9\nB";

    const SourceId id =
        manager.add_source("utf8.torel", source);

    const SourceFile* file = manager.get(id);

    expect(file != nullptr, "UTF-8 source must resolve");

    expect(
        file->size_bytes() == 5,
        "UTF-8 source size must be measured in bytes");

    const SourceSpan multibyte{id, 1, 3};

    const auto text = manager.text(multibyte);

    expect(
        text.has_value(),
        "UTF-8 byte span must resolve");

    expect(
        text->size() == 2,
        "multibyte character must occupy two source bytes");

    expect(
        *text == std::string_view{"\xC3\xA9", 2},
        "UTF-8 span must preserve exact bytes");

    const auto second_line =
        manager.line_column(SourceLocation{id, 4});

    expect(
        second_line.has_value(),
        "second UTF-8 source line must resolve");

    expect(
        second_line->line == 2,
        "byte offset 4 must begin line 2");

    expect(
        second_line->byte_column == 1,
        "line 2 must begin at byte column 1");

    const auto inside_multibyte =
        manager.line_column(SourceLocation{id, 2});

    expect(
        inside_multibyte.has_value(),
        "location inside multibyte sequence must resolve");

    expect(
        inside_multibyte->line == 1,
        "multibyte sequence must remain on line 1");

    expect(
        inside_multibyte->byte_column == 3,
        "columns at this layer must count bytes");
}
void test_empty_source()
{
    using namespace torel::source;

    SourceManager manager;

    const SourceId id =
        manager.add_source("empty.torel", "");

    const SourceFile* file = manager.get(id);

    expect(file != nullptr, "empty source must resolve");
    expect(file->size_bytes() == 0, "empty source size mismatch");
    expect(file->line_count() == 1, "empty source must contain one logical line");

    const auto location =
        manager.line_column(SourceLocation{id, 0});

    expect(location.has_value(), "empty source EOF must resolve");
    expect(location->line == 1, "empty source must be line 1");
    expect(
        location->byte_column == 1,
        "empty source must begin at byte column 1");
}

} // namespace

int main()
{
    try {
        test_source_id();
        test_source_manager_identity();
        test_exact_source_preservation();
        test_line_mapping();
        test_spans();
        test_utf8_byte_semantics();
        test_empty_source();
    }
    catch (const std::exception& exception) {
        std::cerr
            << "source infrastructure test failure: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }

    std::cout
        << "TOREL source infrastructure tests passed\n";

    return EXIT_SUCCESS;
}