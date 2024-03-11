# Dust programming language
Сompiled, simple and imperative programming language. The compiler is made using LLVM. All tests were performed on Ubuntu Linux.

# Was done
- [x] Variables semantic
- [x] Mut statement
- [x] Assign statement
- [x] Exit statement
- [x] Arithmetic operations with precedence
- [X] Wrint line statement
- [X] Const variables
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
Example:
```js
extern std;

mut a = 5;
a = "Hello, world!";

const b = "Dust";

mut c = "string";
c = b;

c = "Test";

writeln(c);
writeln(b);
writeln(2+2*2);

exit(0);
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
