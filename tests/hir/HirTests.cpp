#include "torel/diagnostics/DiagnosticEngine.hpp"
#include "torel/hir/HirKind.hpp"
#include "torel/hir/HirLowerer.hpp"
#include "torel/hir/HirModule.hpp"
#include "torel/hir/NodeId.hpp"
#include "torel/lexer/Lexer.hpp"
#include "torel/parser/Parser.hpp"
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

torel::hir::HirModule lower_source(
    std::string source_text,
    torel::source::SourceManager& sources,
    torel::diagnostics::DiagnosticEngine& diagnostics)
{
    const auto source_id =
        sources.add_source(
            "hir-test.torel",
            std::move(source_text));

    const auto* source_file =
        sources.get(source_id);

    expect(
        source_file != nullptr,
        "HIR test source must resolve");

    torel::lexer::Lexer lexer{
        *source_file,
        diagnostics
    };

    auto tokens =
        lexer.lex_all();

    torel::parser::Parser parser{
        std::move(tokens),
        diagnostics
    };

    auto syntax_tree =
        parser.parse();

    torel::hir::HirLowerer lowerer{
        sources
    };

    return lowerer.lower(syntax_tree);
}

void test_node_id()
{
    using torel::hir::NodeId;

    const NodeId invalid{};

    expect(
        !invalid.is_valid(),
        "default NodeId must be invalid");

    const NodeId node{17};

    expect(
        node.is_valid(),
        "explicit NodeId must be valid");

    expect(
        node.value() == 17,
        "NodeId value mismatch");
}

void test_module_identity()
{
    using namespace torel::hir;

    HirModule module;

    const NodeId first =
        module.add_node(
            HirKind::Root,
            {});

    const NodeId second =
        module.add_node(
            HirKind::Name,
            {},
            "alpha");

    expect(
        first.value() == 0,
        "first HIR NodeId must be zero");

    expect(
        second.value() == 1,
        "second HIR NodeId must be one");

    expect(
        module.node_count() == 2,
        "HIR module node count mismatch");

    expect(
        module.get(NodeId{999}) == nullptr,
        "unknown NodeId must not resolve");
}

void test_node_storage_is_stable()
{
    using namespace torel::hir;

    HirModule module;

    const NodeId root_id =
        module.add_node(
            HirKind::Root,
            {});

    HirNode* original =
        module.get(root_id);

    expect(
        original != nullptr,
        "initial HIR node must resolve");

    for (std::size_t index = 0;
         index < 4096;
         ++index)
    {
        module.add_node(
            HirKind::Name,
            {},
            "temporary");
    }

    HirNode* after_growth =
        module.get(root_id);

    expect(
        after_growth == original,
        "HIR node addresses must remain stable as arena grows");

    expect(
        after_growth->id == root_id,
        "stable HIR node must preserve its NodeId");
}
void test_basic_lowering()
{
    using namespace torel::hir;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto module =
        lower_source(
            "alpha 42 84.6 \"Nova\"",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 0,
        "valid HIR input must produce no diagnostics");

    const HirNode* root =
        module.get(module.root());

    expect(root != nullptr, "HIR root must resolve");

    expect(
        root->kind == HirKind::Root,
        "HIR root kind mismatch");

    expect(
        root->children.size() == 4,
        "HIR root must contain four semantic leaves");

    const HirNode* name =
        module.get(root->children[0]);

    const HirNode* integer =
        module.get(root->children[1]);

    const HirNode* floating =
        module.get(root->children[2]);

    const HirNode* string =
        module.get(root->children[3]);

    expect(
        name != nullptr &&
        name->kind == HirKind::Name &&
        name->text == "alpha",
        "identifier lowering mismatch");

    expect(
        integer != nullptr &&
        integer->kind == HirKind::IntegerLiteral &&
        integer->text == "42",
        "integer lowering mismatch");

    expect(
        floating != nullptr &&
        floating->kind == HirKind::FloatLiteral &&
        floating->text == "84.6",
        "float lowering mismatch");

    expect(
        string != nullptr &&
        string->kind == HirKind::StringLiteral &&
        string->text == "\"Nova\"",
        "string literal must retain raw spelling");
}

void test_scope_lowering()
{
    using namespace torel::hir;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto module =
        lower_source(
            "|: alpha :|",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 0,
        "valid scope must lower without errors");

    const HirNode* root =
        module.get(module.root());

    expect(
        root != nullptr &&
        root->children.size() == 1,
        "HIR root must contain one scope");

    const HirNode* scope =
        module.get(root->children[0]);

    expect(
        scope != nullptr &&
        scope->kind == HirKind::Scope,
        "scope must lower to HIR Scope");

    expect(
        scope->children.size() == 1,
        "scope must contain one HIR child");

    const HirNode* name =
        module.get(scope->children[0]);

    expect(
        name != nullptr &&
        name->kind == HirKind::Name &&
        name->text == "alpha",
        "scope body name lowering mismatch");

    expect(
        scope->span.begin == 0,
        "scope source span begin mismatch");

    expect(
        scope->span.end == 11,
        "scope source span end mismatch");
}

void test_unlowered_syntax_is_explicit()
{
    using namespace torel::hir;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto module =
        lower_source(
            "alpha :: 12",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 0,
        "known but not-yet-lowered syntax must not be an error");

    const HirNode* root =
        module.get(module.root());

    expect(
        root != nullptr &&
        root->children.size() == 3,
        "establishment skeleton must preserve three HIR children");

    const HirNode* middle =
        module.get(root->children[1]);

    expect(
        middle != nullptr &&
        middle->kind == HirKind::UnloweredSyntax,
        ":: must remain explicitly unlowered");

    expect(
        middle->text == "::",
        "unlowered syntax must preserve raw spelling");
}

void test_parser_error_lowers_to_error_node()
{
    using namespace torel::hir;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto module =
        lower_source(
            "alpha :| beta",
            sources,
            diagnostics);

    expect(
        diagnostics.error_count() == 1,
        "parser error must remain diagnosed");

    const HirNode* root =
        module.get(module.root());

    expect(root != nullptr, "HIR root must resolve");

    bool found_error = false;

    for (const NodeId child_id : root->children) {
        const HirNode* child =
            module.get(child_id);

        if (
            child != nullptr &&
            child->kind == HirKind::Error)
        {
            found_error = true;
        }
    }

    expect(
        found_error,
        "CST error node must survive as HIR Error node");
}

void test_preorder_node_ids()
{
    using namespace torel::hir;

    torel::source::SourceManager sources;
    torel::diagnostics::DiagnosticEngine diagnostics;

    auto module =
        lower_source(
            "|: alpha :| beta",
            sources,
            diagnostics);

    const HirNode* root =
        module.get(module.root());

    expect(
        root != nullptr &&
        root->id.value() == 0,
        "HIR root must receive NodeId zero");

    const HirNode* scope =
        module.get(root->children[0]);

    expect(
        scope != nullptr &&
        scope->id.value() == 1,
        "first lowered child must receive NodeId one");

    const HirNode* alpha =
        module.get(scope->children[0]);

    expect(
        alpha != nullptr &&
        alpha->id.value() == 2,
        "scope child must follow scope in preorder");
}

} // namespace

int main()
{
    try {
        test_node_id();
        test_module_identity();
        test_node_storage_is_stable();
        test_basic_lowering();
        test_scope_lowering();
        test_unlowered_syntax_is_explicit();
        test_parser_error_lowers_to_error_node();
        test_preorder_node_ids();
    }
    catch (const std::exception& exception) {
        std::cerr
            << "HIR test failure: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }

    std::cout
        << "TOREL HIR foundation tests passed\n";

    return EXIT_SUCCESS;
}