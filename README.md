# Sink programming language
An incremnted C...C incremented... C-inc? Sink!

## Goals
- A C developer experience with some of the edges filed down.
- Excellent interop with the huge, existing C ecosystem. It's really just C with some sugar on top.
- No classes or inheritance. Let's not just rebuild C++.
- No built-in OOP features.
- NO GC(!!)
- A small, solid standard library for extremely common use cases (e.g. strings, dynamic arrays, serialization, etc.)
- Elimination of header files(? might not be possible at first since this is going to be a thin layer on top of C.)

## MVP Features
- UFCS (Universal Function Call Syntax) - There will be no classes, so methods aren't really a thing. Sometimes it's nice to call a function with method syntax though.
- Type Inferrence - Functions have return types...why not let them work for us?
- Multiple Return Values - à la Go.
- Type aliasing.
- Function overloading.
- Compile to C and allow gcc or clang backends.

