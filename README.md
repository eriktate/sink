# Sink
An incremnted C...C incremented... C-inc? Sink!

## Initial Goals
- A C developer experience with some of the edges filed down.
- Excellent interop with the huge, existing C ecosystem. It's really just C with some sugar on top.
- No classes, inheritance, templates, or bloat. Let's not just rebuild C++.
- No built-in OOP features.
- NO GC(!!)

## MVP Features
- UFCS (Universal Function Call Syntax) - There will be no classes, so methods aren't really a thing. Sometimes it's nice to call a function with method syntax though.
- Type Inferrence - Functions have return types...why not let them work for us?
- Function overloading.
- Compile to C and allow gcc or clang backends.

## In the far off future...
- New syntax.
- LLVM backend.
- A small, solid standard library for extremely common use cases (e.g. strings, dynamic arrays, serialization, etc.)
- Multiple Return Values - à la Go.
- Interfaces (maybe?)
- Elimination of header files/more modern packaging.
