# C-Like Compiler Project(SOC 2025)

- This is my project for the Seasons Of Code 2025.
- A toy compiler for a C-like programming language built entirely using c++
- The compiler supports Variable, Function declarations and initialisation. The compiler currently only supports int datatype.
- Scope tracking is supported along with features like if-else statements and while loops.
- At the current stage the implementation includes a lexer that can create tokens from the input code
- A parser that generates a Parse Syntax tree and prints it to the console and checks for syntax errors
- A semantic analyser that analyses The parsed code and looks for logical errors.

## What i have learned so far?

- **Tokenisation**: How to break input source code into tokens (identifiers, numbers, keywords, symbols),Handling whitespace and new line charachters efficiently.
<pre>

// Keywords
int, void, if, else, while, return, input, output

// Operators  
+, -, *, /, =, ==, !=, <, <=, >, >=

// Delimiters
{, }, (, ), [, ], ;, ,

// Literals & Identifiers
integers, variable names, function names

These were all the tokens that were used in the lexer code.
</pre>

The language uses a rather simple syntax and token set.

- **Parsing Tokens to create an Abstract Syntax Tree**: This part of the project really helped me solidfy my concepts of c++ polymorphism and recursion. The algorithm used to generate a parse-tree is largely a recursive algorithm and i really had a lot of fun implementing it.
<pre>
program        → declaration-list
declaration    → var-declaration | fun-declaration
var-declaration → type-specifier ID ';' | type-specifier ID '[' NUM ']' ';'
fun-declaration → type-specifier ID '(' params ')' compound-stmt
type-specifier → 'int' | 'void'
compound-stmt  → '{' local-declarations statement-list '}'
statement      → expression-stmt | compound-stmt | selection-stmt | 
                 iteration-stmt | return-stmt
expression     → var '=' expression | simple-expression
simple-expression → additive-expression relop additive-expression
factor         → '(' expression ')' | var | call | NUM
</pre>
this the grammer that was used for the parse tree algorithm.
the program is essentially a list of function and variable declarations. We loop through each of these declarations and parse them recursively.

**An Virtual ASTNode class was implemented**
```cpp
class ASTNode{
    public:
        virtual ~ASTNode() = default;
        virtual void print(int indent = 0) const = 0;
        virtual void accept(Visitor& visitor) = 0;
};

