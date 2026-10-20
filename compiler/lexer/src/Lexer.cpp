#include "torel/lexer/Lexer.hpp"

#include "torel/diagnostics/Diagnostic.hpp"
#include "torel/diagnostics/DiagnosticCode.hpp"
#include "torel/diagnostics/DiagnosticLabel.hpp"
#include "torel/diagnostics/DiagnosticSeverity.hpp"

#include <cctype>
#include <cstddef>
#include <string>
#include <utility>

namespace torel::lexer {

namespace {

bool is_identifier_start(char value) noexcept
{
    const auto byte =
        static_cast<unsigned char>(value);

    return std::isalpha(byte) != 0 ||
           value == '_';
}

bool is_identifier_continue(char value) noexcept
{
    const auto byte =
        static_cast<unsigned char>(value);

    return std::isalnum(byte) != 0 ||
           value == '_';
}

bool is_decimal_digit(char value) noexcept
{
    return value >= '0' && value <= '9';
}

} // namespace

Lexer::Lexer(
    const source::SourceFile& source_file,
    diagnostics::DiagnosticEngine& diagnostics)
    : source_file_(source_file),
      diagnostics_(diagnostics),
      source_(source_file.contents())
{
}

std::vector<Token> Lexer::lex_all()
{
    std::vector<Token> tokens;

    while (!at_end()) {
        skip_whitespace();

        if (at_end()) {
            break;
        }

        const char value = current();

        if (is_identifier_start(value)) {
            tokens.push_back(lex_identifier());
            continue;
        }

        if (is_decimal_digit(value)) {
            tokens.push_back(lex_number());
            continue;
        }

        if (value == '"') {
            tokens.push_back(lex_string());
            continue;
        }

        tokens.push_back(lex_punctuation());
    }

    tokens.push_back(
        make_token(
            TokenKind::EndOfFile,
            offset_,
            offset_));

    return tokens;
}

bool Lexer::at_end() const noexcept
{
    return offset_ >= source_.size();
}

char Lexer::current() const noexcept
{
    return at_end()
        ? '\0'
        : source_[offset_];
}

char Lexer::peek(std::size_t distance) const noexcept
{
    const std::size_t target =
        offset_ + distance;

    if (target >= source_.size()) {
        return '\0';
    }

    return source_[target];
}

void Lexer::advance(std::size_t count) noexcept
{
    offset_ += count;

    if (offset_ > source_.size()) {
        offset_ = source_.size();
    }
}

void Lexer::skip_whitespace() noexcept
{
    while (!at_end()) {
        const char value = current();

        if (
            value == ' ' ||
            value == '\t' ||
            value == '\n' ||
            value == '\r')
        {
            advance();
            continue;
        }

        break;
    }
}

Token Lexer::lex_identifier()
{
    const std::size_t begin = offset_;

    advance();

    while (
        !at_end() &&
        is_identifier_continue(current()))
    {
        advance();
    }

    return make_token(
        TokenKind::Identifier,
        begin,
        offset_);
}

Token Lexer::lex_number()
{
    const std::size_t begin = offset_;

    while (
        !at_end() &&
        is_decimal_digit(current()))
    {
        advance();
    }

    TokenKind kind =
        TokenKind::IntegerLiteral;

    if (
        !at_end() &&
        current() == '.' &&
        is_decimal_digit(peek()))
    {
        kind = TokenKind::FloatLiteral;
        advance();

        while (
            !at_end() &&
            is_decimal_digit(current()))
        {
            advance();
        }
    }

    return make_token(
        kind,
        begin,
        offset_);
}

Token Lexer::lex_string()
{
    const std::size_t begin = offset_;

    advance();

    while (
        !at_end() &&
        current() != '"')
    {
        advance();
    }

    if (at_end()) {
        report_unterminated_string(
            begin,
            offset_);

        return make_token(
            TokenKind::Invalid,
            begin,
            offset_);
    }

    advance();

    return make_token(
        TokenKind::StringLiteral,
        begin,
        offset_);
}

Token Lexer::lex_punctuation()
{
    struct Entry final {
        std::string_view spelling;
        TokenKind kind;
    };

    static constexpr Entry entries[] = {
        {":?&", TokenKind::RuntimeRequire},
        {":/?", TokenKind::CancelCheckpoint},

        {"::", TokenKind::Establish},
        {":~", TokenKind::Alter},
        {":?", TokenKind::QuestionBranch},
        {":!", TokenKind::AlternateBranch},
        {":>", TokenKind::Feed},
        {":<", TokenKind::ObserveContact},
        {":%", TokenKind::AlterContact},
        {":.", TokenKind::Sever},
        {":+", TokenKind::Extend},
        {":&", TokenKind::Require},

        {"#:", TokenKind::TypeDefine},
        {"&:", TokenKind::CapabilityDefine},

        {"!!", TokenKind::Rupture},
        {"!?", TokenKind::InterceptRupture},
        {"!&", TokenKind::Breach},

        {":*", TokenKind::SplitExecution},
        {"*:", TokenKind::ConvergeExecution},
        {":/", TokenKind::Cancel},
        {"*?", TokenKind::CompletionInspect},

        {"~:", TokenKind::RepeatBackedge},
        {"^:", TokenKind::CompleteOutward},
        {":@", TokenKind::Traverse},

        {"|:", TokenKind::ScopeOpen},
        {":|", TokenKind::ScopeClose},

        {"<|", TokenKind::PortOpen},
        {"||", TokenKind::PortSeparator},
        {"|>", TokenKind::PortClose},

        {"[.", TokenKind::GroupOpen},
        {".]", TokenKind::GroupClose},

        {"[|", TokenKind::ShapeOpen},
        {"|]", TokenKind::ShapeClose},

        {"[:", TokenKind::RealizationOpen},
        {":]", TokenKind::RealizationClose},

        {"\\:", TokenKind::VariantMarker},
        {"[?", TokenKind::ExposureOpen},
        {"?]", TokenKind::ExposureClose},

        {"##", TokenKind::TypeParameter},

        {"<=", TokenKind::LessEqual},
        {">=", TokenKind::GreaterEqual},

        {"#", TokenKind::Hash},
        {"?", TokenKind::Question},
        {"~", TokenKind::Tilde},
        {"!", TokenKind::Bang},
        {"@", TokenKind::At},
        {"&", TokenKind::Ampersand},
        {"^", TokenKind::Caret},

        {"+", TokenKind::Plus},
        {"-", TokenKind::Minus},
        {"*", TokenKind::Star},
        {"/", TokenKind::Slash},
        {"%", TokenKind::Percent},

        {"<", TokenKind::Less},
        {">", TokenKind::Greater},

        {".", TokenKind::Dot},
        {";", TokenKind::Semicolon}
    };

    const std::size_t begin = offset_;

    for (const Entry& entry : entries) {
        if (!matches(entry.spelling)) {
            continue;
        }

        advance(entry.spelling.size());

        return make_token(
            entry.kind,
            begin,
            offset_);
    }

    report_unexpected_byte(offset_);
    advance();

    return make_token(
        TokenKind::Invalid,
        begin,
        offset_);
}

Token Lexer::make_token(
    TokenKind kind,
    std::size_t begin,
    std::size_t end) const noexcept
{
    return Token{
        kind,
        source::SourceSpan{
            source_file_.id(),
            static_cast<source::ByteOffset>(begin),
            static_cast<source::ByteOffset>(end)
        }
    };
}

void Lexer::report_unexpected_byte(
    std::size_t offset)
{
    using namespace diagnostics;

    Diagnostic diagnostic{
        DiagnosticSeverity::Error,
        DiagnosticCode{"LEX0001"},
        "unexpected byte in TOREL source",
        {},
        {}
    };

    diagnostic.labels.push_back(
        DiagnosticLabel{
            source::SourceSpan{
                source_file_.id(),
                static_cast<source::ByteOffset>(offset),
                static_cast<source::ByteOffset>(offset + 1)
            },
            DiagnosticLabelKind::Primary,
            "this byte does not begin a recognized token"
        });

    diagnostics_.emit(std::move(diagnostic));
}

void Lexer::report_unterminated_string(
    std::size_t begin,
    std::size_t end)
{
    using namespace diagnostics;

    Diagnostic diagnostic{
        DiagnosticSeverity::Error,
        DiagnosticCode{"LEX0002"},
        "unterminated string literal",
        {},
        {}
    };

    diagnostic.labels.push_back(
        DiagnosticLabel{
            source::SourceSpan{
                source_file_.id(),
                static_cast<source::ByteOffset>(begin),
                static_cast<source::ByteOffset>(end)
            },
            DiagnosticLabelKind::Primary,
            "string literal begins here but has no closing quote"
        });

    diagnostics_.emit(std::move(diagnostic));
}

bool Lexer::matches(
    std::string_view text) const noexcept
{
    if (
        offset_ + text.size() >
        source_.size())
    {
        return false;
    }

    return source_.substr(
        offset_,
        text.size()) == text;
}

} // namespace torel::lexer