# Security Policy

TOREL is experimental. No current release or branch should be treated as a production-ready compiler, runtime, or security boundary.

## Supported versions

There is not yet a security-stable TOREL release.

Security reports against the current development branch are still valuable and are accepted.

## Reporting a vulnerability

Do **not** publish exploitable details in a normal public GitHub issue.

Use GitHub private vulnerability reporting / Security Advisories for this repository when available:

```text
https://github.com/ByteCraft-Co/TOREL/security/advisories/new
```

A useful report should include:

- affected commit or revision;
- platform and architecture;
- minimal reproduction;
- observed behavior;
- expected behavior;
- likely security impact;
- whether the issue requires malformed source, unsafe code, foreign code, or ordinary safe TOREL code.

Please avoid including secrets or unrelated personal information in reports.

## Security model under development

TOREL's language design targets a safe subset in which ordinary safe code prevents, rejects, or turns into defined failure states such problems as:

- dangling contacts;
- use-after-surrender or use-after-destruction;
- double destruction or double free through safe APIs;
- reads of uninitialized typed values;
- invalid variant payload access;
- ordinary out-of-bounds memory access;
- invalid typed reinterpretation;
- overlapping safe mutable aliases;
- ordinary non-atomic data races;
- hidden null dereferences;
- undeclared recoverable failure.

These are **design targets**, not claims that the current experimental compiler already enforces the full model.

## Unsafe and foreign boundaries

The long-term design treats unsafe operations as local, explicit, and contractual. Foreign code does not automatically inherit TOREL safety guarantees.

Security bugs in the compiler, runtime, unsafe abstractions, or foreign-interface boundaries are therefore in scope even when a program would eventually require an explicit unsafe boundary.

## Supply-chain reports

Reports involving CI, dependency integrity, release artifacts, build inputs, or repository automation are also in scope.
