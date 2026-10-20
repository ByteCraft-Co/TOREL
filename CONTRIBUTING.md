# Contributing to TOREL

TOREL is an experimental programming language and compiler project. Contributions should preserve the distinction between language design, compiler implementation, and repository infrastructure.

## Design authority

Compiler behavior is not automatically language law.

When project documents disagree, contributors should follow the current design-authority order:

1. the TOREL/Genesis Constitution;
2. accepted entries in the decision register;
3. frozen or candidate language specifications;
4. domain specifications;
5. IR/runtime/ABI/performance plans;
6. compiler implementation.

An implementation shortcut must not silently redefine the language.

Accepted design decisions should be superseded explicitly rather than rewritten as if the earlier decision never existed.

## Branch workflow

Do not develop directly on `main`.

Start from an up-to-date `main` and create a focused branch:

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

A branch should represent one reviewable engineering change.

## Before opening a pull request

At minimum:

1. configure successfully;
2. build successfully;
3. run the complete test suite;
4. run `git diff --check`;
5. review the staged diff;
6. keep unrelated work out of the branch.

Typical local commands:

```powershell
cmake --preset local-dev
cmake --build --preset local-dev
ctest --preset local-dev
git diff --check
```

## Pull requests

A pull request should explain:

- what changed;
- why it is needed;
- which architectural boundary is affected;
- which invariants are introduced or preserved;
- which tests were added or changed;
- which work is deliberately deferred.

Required CI checks must pass before merge.

## Language-design changes

Language design must not change accidentally inside an implementation PR.

A language-design proposal should state, in this order:

1. the semantic problem;
2. the proposed semantic rule;
3. syntax, if syntax is involved;
4. interaction with existing rules;
5. safety and performance implications;
6. alternatives considered;
7. proposed stability status.

Do not introduce syntax merely because another language uses it.

If an accepted decision changes, the decision history and affected tests/specifications should be updated explicitly.

## Compiler architecture

The intended dependency direction is broadly:

```text
support
  ↑
source
  ↑
lexer
  ↑
parser / syntax
  ↑
modules / resolution
  ↑
HIR
  ↑
types / proofs
  ↑
authority / control
  ↑
HLIR
  ↑
MIR
  ↑
optimizer
  ↑
backend
  ↑
driver
```

Avoid reverse dependencies and cross-layer shortcuts.

## Testing

Invalid programs are first-class test inputs.

Every correctness fix should add a regression test when practical. Tests should validate language/compiler invariants rather than current implementation accidents.

Compiler crashes on malformed source are bugs.

## Formatting

C++ code follows `.clang-format`.

Repository text follows `.editorconfig` and `.gitattributes`.

Formatting changes should not be mixed with unrelated semantic changes unless unavoidable.

## Issues and deferred work

If implementation reveals legitimate work outside the current branch, create or update a GitHub issue instead of expanding the branch indefinitely.

Use the structured issue forms for compiler bugs, compiler features, and language-design work.
