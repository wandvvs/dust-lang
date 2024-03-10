# Dust programming language
Сompiled, simple and imperative programming language. The compiler is made using LLVM. All tests were performed on Ubuntu Linux.

# Was done
- [x] Variables semantic
- [x] Let statement
- [x] Assign statement
- [x] Exit statement
- [x] Arithmetic operations with precedence
- [ ] Print statement
- [ ] Const variables
- [ ] Data types
- [ ] If statement
- [ ] Loop statement
- [ ] Etc...

## Getting started
Dependencies: LLVM-DEV, LLVM, C++20, GTest (not necessary)
1. Clone repository:
```bash
git clone https://github.com/wandvvs/dust-lang
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
## Grammar
```
program : statement_list

statement_list : statement
               | statement_list statement

statement : let_statement
          | assign_statement
          | exit_statement

let_statement : LET IDENTIFIER '=' expression ';'

assign_statement : IDENTIFIER '=' expression ';'

exit_statement : EXIT '(' expression ')' ';'

expression : term
           | expression '+' term
           | expression '-' term

term : factor
     | term '*' factor
     | term '/' factor

factor : INT_LITERAL
       | IDENTIFIER
       | '(' expression ')'
```

## Example
```
let a = 5;
let b = (a+5)/2;

b = b + 3;

exit(b*2);
```

Let`s compile it and run
```bash
./dust-lang input.dust
./out
echo $?
```

LLVM IR:
```llvm
define i64 @main() {
entrypoint:
  ret i64 16
}
```

Output:
```
16
```
