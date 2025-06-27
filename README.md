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
