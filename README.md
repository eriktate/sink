# Sink
An incremented C...C incremented... C-inc? Sink!

## Goals
- A C-like developer experience with some nice quality of life improvements.
- Excellent interop with the huge, existing C ecosystem. For Sink to be at all useful, we can't ignore the wealth of prior art in systems programming.
- No classes, inheritance, templates, or bloat. Let's not just rebuild C++.
- No built-in OOP features.
- NO GC(!!)

## MVP Features
- UFCS (Universal Function Call Syntax) - There will be no classes, so methods aren't really a thing. Sometimes it's nice to call a function with method syntax though.
- Type Inferrence - Functions have return types...why not let them work for us?
- Tuples (multiple return).
- Function overloading.
- Operator overloading.
- LLVM backend.
- No forward declarations for functions or structs.
- A small, solid standard library for extremely common use cases (e.g. strings, dynamic arrays, serialization, etc.)
- Elimination of header files with more modern packaging.
- Actually helpful error messages.

## Maybe features
- First class functions
- Higher order functions.
- Closures.
- Something *like* interfaces (?)
