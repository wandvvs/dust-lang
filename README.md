# Dust programming language
Сompiled, simple imperative programming language created for the purpose of education with support for basic variable declarations, assignments, expressions, printing, and program termination.
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
### Download actual release from [dust language releases](https://github.com/wandvvs/dust-lang/releases/tag/dust_lang_0_0_3) 

## Example
```js
extern std;

mut a = false;
a = "from boolean to str";

writeln(a);

const b = 3.14 * (5/2) + 5.1;
writeln(b);

mut str = "from str to int";
str = 5.8*1.3;

writeln(str);

writeln(true);

exit(5);
```

```bash
./dust-lang <input.dust>
./out
```

Output:
```
from boolean to str
12.950000
7.540000
true
```
