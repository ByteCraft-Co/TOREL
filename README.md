# TOREL

[![CI](https://github.com/ByteCraft-Co/TOREL/actions/workflows/ci.yml/badge.svg)](https://github.com/ByteCraft-Co/TOREL/actions/workflows/ci.yml)

**TOREL — Typed Ownership and Relational Execution Language**

TOREL is an experimental statically typed native programming language and compiler project.

The project is designed around explicit relationships, explicit authority, deterministic semantics, safe native execution without a mandatory tracing garbage collector, strong diagnostics, and compiler-owned intermediate representations that keep the language independent of any particular backend.

> TOREL is under active development. It is not yet a production-ready language or compiler.

## Current status

The permanent Stage 0 compiler is written in C++23. The repository currently contains foundations for:

- source-file ownership and byte-based source spans;
- structured diagnostics;
- longest-match lexical analysis;
- a token-preserving structural CST parser;
- an initial HIR arena and CST-to-HIR lowering boundary;
- CMake + Ninja builds;
- LLVM 23 integration;
- automated Linux CI.

An earlier disposable experimental compiler proved that the planned architecture can reach native execution. That experiment is evidence for the architecture, not the production compiler and not the language specification.

TOREL has not reached language version 0.1. Syntax and compiler interfaces may still change when specification or implementation evidence requires it.

## Design principles

TOREL is designed semantics-first:

```text
Semantics first.
Syntax second.
Existing languages last.
```

Important project principles include:

- ownership transfer must be visible;
- mutation authority must be visible;
- recoverable failure must be declared;
- safe code must not depend on undefined behavior;
- evaluation order is language-defined;
- optimization must preserve semantics;
- creation and mutation are distinct operations;
- dynamic cost should be visible;
- LLVM is replaceable implementation infrastructure;
- the compiler is not the specification;
- strict language rules require strong diagnostics.

## Compiler architecture

The intended compiler progression is:

```text
TOREL source
    ↓
Source Manager
    ↓
Lexer
    ↓
Parser / CST
    ↓
HIR
    ↓
Name Resolution
    ↓
Type / Proof / Authority Analysis
    ↓
TOREL HLIR
    ↓
TOREL MIR
    ↓
Backend
    ↓
Native Code
```

Only the early stages are currently implemented in the permanent compiler.

## Repository layout

The project keeps major concerns separate:

```text
compiler/   Stage 0 compiler
runtime/    Runtime components
stdlib/     TOREL standard library
tools/      Non-compiler tooling
tests/      Compiler and conformance tests
examples/   Maintained example programs
docs/       Human-oriented documentation
spec/       Normative language/project specifications
bootstrap/  Bootstrap and reproducibility material
```

Not every planned directory is populated yet.

## Building

### Requirements

- CMake 3.25 or newer
- Ninja
- a C++23-capable compiler
- LLVM 23 development files

The committed `CMakePresets.json` is machine-independent. Developer-specific compiler and LLVM paths belong in `CMakeUserPresets.json`, which is intentionally ignored by Git.

A template is provided as:

```text
CMakeUserPresets.json.example
```

Copy it to `CMakeUserPresets.json`, edit the local paths, then use:

```powershell
cmake --preset local-dev
cmake --build --preset local-dev
ctest --preset local-dev
```

CI uses the committed `ci-linux` preset.

## Development model

Development happens on focused branches and enters `main` through pull requests.

Typical branch families:

```text
compiler/...
runtime/...
stdlib/...
tooling/...
spec/...
repo/...
fix/...
refactor/...
```

Every change to `main` is expected to pass the repository CI gate.

See [CONTRIBUTING.md](CONTRIBUTING.md) for the engineering workflow.

## Project status

See [docs/STATUS.md](docs/STATUS.md).

## Security

See [SECURITY.md](SECURITY.md).

## License

TOREL is licensed under the [Apache License 2.0](LICENSE).
