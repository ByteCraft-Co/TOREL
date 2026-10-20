#pragma once

#include <cstdint>

namespace torel::lexer {

enum class TokenKind : std::uint16_t {
    Invalid,
    EndOfFile,

    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,

    // Core relational operators.
    Establish,              // ::
    Alter,                  // :~
    QuestionBranch,         // :?
    AlternateBranch,        // :!
    Feed,                   // :>
    ObserveContact,         // :<
    AlterContact,           // :%
    Sever,                  // :.
    Extend,                 // :+
    Require,                // :&
    RuntimeRequire,         // :?&

    TypeDefine,             // #:
    CapabilityDefine,       // &:

    Rupture,                // !!
    InterceptRupture,       // !?
    Breach,                 // !&

    SplitExecution,         // :*
    ConvergeExecution,      // *:
    Cancel,                 // :/
    CancelCheckpoint,       // :/?
    CompletionInspect,      // *?

    RepeatBackedge,         // ~:
    CompleteOutward,        // ^:
    Traverse,               // :@

    ScopeOpen,              // |:
    ScopeClose,             // :|

    PortOpen,               // <|
    PortSeparator,          // ||
    PortClose,              // |>

    GroupOpen,              // [.
    GroupClose,             // .]

    ShapeOpen,              // [|
    ShapeClose,             // |]

    RealizationOpen,        // [:
    RealizationClose,       // :]

    VariantMarker,          // \:
    ExposureOpen,           // [?
    ExposureClose,          // ?]

    TypeParameter,          // ##

    LessEqual,              // <=
    GreaterEqual,           // >=

    Hash,                   // #
    Question,               // ?
    Tilde,                  // ~
    Bang,                   // !
    At,                     // @
    Ampersand,              // &
    Caret,                  // ^

    Plus,                   // +
    Minus,                  // -
    Star,                   // *
    Slash,                  // /
    Percent,                // %

    Less,                   // <
    Greater,                // >

    Dot,                    // .
    Semicolon               // ;
};

} // namespace torel::lexer