# Dust programming language
Сompiled, simple and imperative programming language. The compiler is made using LLVM. All tests were performed on Ubuntu Linux.

# Was done
- [x] Variable semantic
- [x] Let statement
- [x] Assign statement
- [x] Exit statement
- [ ] Print statement
- [ ] Const variables
- [ ] Adding and multiplication with priority
- [ ] If statement
- [ ] Loop statement
- [ ] Functions statement
- [ ] Etc...

## Getting started
Dependencies: LLVM-DEV, LLVM, C++20, GTest (not necessary)
1. Clone repository:
```bash
https://github.com/wandvvs/dust-lang
```
2. Choose dust-lang directory and build it
```bash
cd dust-lang/
cd build/
cmake ..
make
```
3. Test language
```bash
./dust-lang <input.dust>
```
## Grammer
```
program ::= statement_list

statement_list ::= statement | statement_list statement

statement ::= let_statement | assign_statement | exit_statement

let_statement ::= "let" identifier "=" expression ";"

assign_statement ::= identifier "=" expression ";"

exit_statement ::= "exit" "(" expression ")" ";"

expression ::= identifier | literal

literal ::= integer_literal

identifier ::= [a-zA-Z]+

integer_literal ::= [0-9]+
```
