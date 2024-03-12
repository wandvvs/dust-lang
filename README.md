# Dust programming language
Сompiled, simple imperative programming language with support for basic variable declarations, assignments, expressions, printing, and program termination.
The compiler is made using LLVM.
When building the compiler, i didn`t implement AST, as many people do.
All tests were performed on Ubuntu Linux.

# Was done
- [x] Variables semantic
- [x] Mut statement
- [x] Assign statement
- [x] Exit statement
- [x] Arithmetic operations with precedence
- [X] Wrint line statement
- [X] Const variables
- [X] String variables
- [X] Bool variables
- [X] Float variables
- [ ] Logical operators
- [ ] If statement
- [ ] Loop statement
- [ ] Math functions for extern std
- [ ] Etc...

## Getting started
### Build it yourself
Dependencies: LLVM-DEV, LLVM, C++20, GTest (not necessary)
```bash
git clone https://github.com/wandvvs/dust-lang
cd dust-lang/
cd build/
cmake ..
make
```
### Download actual release from [dust language releases](https://github.com/wandvvs/dust-lang/releases/tag/dust_lang2) 

## Example
```js
extern std;

mut a = 3.14 * (5/2) + 5.1;

writeln(a);

const b = true;
writeln(b);

mut a = false;
a = "from boolean to str";

writeln(a);

mut f = "str";
f = false;
writeln(a);

f = (5+2) * 2 / 2;

writeln(f);

exit(5);
```

```bash
./dust-lang <input.dust>
./out
```

Output:
```
Test
string
6
1333 333 333
```
