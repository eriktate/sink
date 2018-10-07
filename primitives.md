# List of primitive types

void - a no-op type.
void * - a no-op pointer. Treated the same as a u8 pointer.

i8  - 8-bit signed integer
i16 - 16-bit signed integer
i32 - 32-bit signed integer
i64 - 64-bit signed integer

u8  - 8-bit unsigned integer
u16 - 16-bit unsigned integer
u32 - 32-bit unsigned integer
u64 - 64-bit unsigned integer

f32 - 32-bit floating point number
f64 - 64-bit floating point number

byte - alias for u8
char - alias for u8 (maybe omit this...?)

codepoint - alias for i32. Meant to represent utf-8 byte sequences.
string  - utf-8 encoded string type. Under the hood, a vec<codepoint>
cstring - null terminated array of bytes.


vec<type> - a vector (dynamically sized, heap-allocated array) of the type given. A triplet of length, capacity, and data pointer.
type[size] - A constant size array of the type given. Same triplet as vec, but stack allocated and not growable. If declared but not initialized, length will be zero.
