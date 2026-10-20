# TOREL Development Status

This document tracks implementation status rather than design status alone.

## Permanent Stage 0 compiler

Implemented foundations:

- C++23 Stage 0 compiler project;
- CMake + Ninja build system;
- LLVM 23 development integration;
- stable source-file identities;
- byte-based source locations and half-open spans;
- preserved source buffers and line indexing;
- structured diagnostic objects and stable diagnostic-code foundation;
- lexer foundation with longest-match structural punctuation;
- structural, token-preserving CST parser;
- explicit scope parsing and basic parser recovery;
- HIR node arena with stable `NodeId` identities;
- CST-to-HIR lowering foundation;
- Linux CI.

## Experimental predecessor

Before the permanent C0 architecture, a disposable experimental compiler exercised an end-to-end path from source through parsing, resolution, a tiny semantic checker, experimental HLIR/MIR0, LLVM IR, and native execution.

That experiment validated architectural feasibility. It is not the production compiler, final MIR, public ABI, or language specification.

## Current engineering direction

The permanent compiler is intentionally being hardened before its feature count expands.

Near-term work:

1. implement establishment syntax through CST and HIR;
2. introduce declaration/reference distinctions required for name resolution;
3. implement the first permanent resolver subset;
4. continue toward the typed semantic core;
5. preserve explicit diagnostics and source provenance at every stage.

## Planned later stages

- name resolution;
- primitive type representation and checking;
- authority analysis;
- proof/capability resolution;
- ownership/contact analysis;
- control-flow and output validation;
- constant evaluation;
- TOREL HLIR;
- SSA-oriented TOREL MIR;
- optimizer;
- LLVM lowering;
- runtime profiles;
- standard library;
- package/build tooling;
- official formatter;
- documentation generator;
- self-hosting.

## Deliberately not stable

The following are not public stability promises:

- language version 0.1;
- public ABI;
- final package manifest syntax;
- final unsafe/FFI surface;
- final comment/string/Unicode lexical policy;
- final MIR representation;
- self-hosting;
- additional backends.

## Stability rule

Implementation does not redefine the language.

A compiler behavior that conflicts with the governing specification is a compiler bug or an explicitly proposed specification change.
