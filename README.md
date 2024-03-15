# Dust programming language
Сompiled, simple imperative programming language created for the purpose of education with support for basic variable declarations, assignments, expressions, printing, program termination like exit() in C language and logical operators.
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
- [x] Logical operators
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
### Download actual release from [dust language releases](https://github.com/wandvvs/dust-lang/releases/tag/dust_lang_0_0_3) 

## Example
```js
use io;

const a = 10 + 2;
mut b = (5 * 2) + 2;

const aEqualB = ? a == b;
writeln(aEqualB);

const PI = 3.14;
const EI = 1.57;

const piLessThanEI = ? PI < EI;
writeln(piLessThanEI);

mut test = ? PI == EI * 2;
writeln(test);

test = "froom boolean to str";
writeln(test);

test = ? 1.57 * 2 == PI;
writeln(test);
```

```bash
./dust-lang <input.dust>
./out
```

Output:
```
true
false
true
froom boolean to str
true
```
