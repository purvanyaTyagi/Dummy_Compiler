# C-Like Compiler Project(SOC 2025)

- This is my project for the Seasons Of Code 2025.
- A toy compiler for a C-like programming language built entirely using c++
- The compiler supports Variable, Function declarations and initialisation. The compiler currently only supports int datatype.
- Scope tracking is supported along with features like if-else statements and while loops.
- At the current stage the implementation includes a lexer that can create tokens from the input code
- A parser that generates a Parse Syntax tree and prints it to the console and checks for syntax errors
- A semantic analyser that analyses The parsed code and looks for logical errors.

## Project Structure 
<pre>
/build - contains compiled object files and executables
/src - contains the project source code
/tests - contains input.txt file to write code that is compiled by the dummy compiler
Makefile
</pre>

## How to run the project?
- Clone the directory into your machine
- The code that needs to be compiled by the dummy compiler goes in test/input.txt
<pre>
make test
</pre>
this will automatically build the project and run it on the code provided in input.txt

you should see a pretty printed syntax tree as an output along with error longs if there are any errors.

```cpp
int x;

int main(int x, int y){
    int z;
    z = 3;
    int k;
    k = input((x + y) + z * 4);
    output(k + x);
}
```

this is a sample code that will run on the current stage of the compiler.

Note: The compilers semantic analyser is not yet complete and hence there are bugs and issues to be fixed as of now.

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
```
multiple other classes inherited this base class. Each child class had its own implementation of the print and accept functions. A default destructor was used for all.

These child classes include classes like, While node, If node, variable declaration node etc.

The nodes also implement a print function that is usefull in pretty printing the Abstract syntax tree.

here is an example,
<pre>
Parser Initialised
Variable: int x
function: int main body: 
  Variable: int z
  Variable initialisation: z = 
    Number: 3
  Variable: int k
  Variable initialisation: k = 
    input expression: 
      Binary Expression: 
        Binary Expression: 
          Variable: x
          operation: +
          Variable: y
        operation: +
        Binary Expression: 
          Variable: z
          operation: *
          Number: 4
  output expression: 
    Binary Expression: 
      Variable: k
      operation: +
      Variable: x
</pre>

this is the generated parse tree for the following code,

```cpp
int x;

int main(int x, int y){
    int z;
    z = 3;
    int k;
    k = input((x + y) + z * 4);
    output(k + x);
}
```

- **Smart pointers**: All object were declared as Unique_ptrs which is a smart_pointer provided by the c++ standard library. This is efficient as unique_ptrs avoid multiple pointers sharing a memory address, This avoids hogging memory, a smart pointer automatically calls destructors of the objects and frees memory when the object is no longer is in use. I learned a lot about how smart pointers work and move semantics in c++ while working on this project.

- **Semantic Analysis**: The ASTNode classes have a function called Accept. which take in a reference to an object of type Visitor. The visitor class was implemented like this.

```cpp
class Visitor {
    public:
        virtual void visit(Var_Decal_Node* node) = 0;
        virtual void visit(Var_Arr_Decal_Node* node) = 0;
        virtual void visit(Func_Decal_Node* node) = 0;
        virtual void visit(Var_Initialise_Node* node) = 0;
        virtual void visit(While_Node* node) = 0;
        virtual void visit(Else_Node* node) = 0;
        virtual void visit(If_Node* node) = 0;
        virtual void visit(Number_Expr* node) = 0;
        virtual void visit(Variable_Expr* node) = 0;
        virtual void visit(Binary_Expr* node) = 0;
        virtual void visit(Assignment_Expr* node) = 0;
        virtual void visit(Rel_Operation* node) = 0;
        virtual void visit(return_node* node) = 0;
        virtual void visit(input_node* node) = 0;
        virtual void visit(output_node* node) = 0;
        virtual ~Visitor() = default;
};
```
this class has a visit function for each node in the **Abstract Syntax Tree**. Now the use of this class is that any another class can inherit from this virtual class and implement these visit functions.
Then these functions can be called recursively to traverse every node in the Syntax tree, What to do when you visit a node can be defined by unique implementations that the inherited class can make.

This allows different child class of the visitor class to traverse the Syntax tree and perform unique tasks with the data recieved. 

This also prevents implementing every single functionality in the Node class. We can just create a child of the virtual class for every new functionality we want.

- **Symbol table generation**: To track variable declarations and inilisation, we create a child of the visitor class and use algorithms that crate a symbol table for this task. 

```cpp
class symbol_table{
    std::vector<Scope> scopes;
    std::vector<std::unordered_map<std::string, bool>> initialised_variables; 
    int current_scope;
    public:
        symbol_table(){
            current_scope = 0;
            enter_scope();
        }
        void enter_scope();
        void exit_scope();

        bool declare(const std::string& name, const SymbolInfo& info);
        bool initialise(const std::string& name);
        SymbolInfo* lookup(const std::string& name);
        bool isDeclaredInCurrentScope(const std::string& name);
        bool isInitialised(const std::string& name);
        void contruct_symbol_table(const std::vector<std::unique_ptr<ASTNode>>& parsed_code); 
        void print();
        std::vector<Scope> return_symbol_table();
};
```

A vector<> was used which kept a scope object. This vector was used like a stack to keep track of variable scopes and ensure that no two variables of the same name were declared in the same scope.

## Current status of the project

- Lexing and tokenization - done

- Parsing into AST - done

- Symbol table implementation - done

- Semantic analysis - Still need to fine tune and implement some more grammar logic of the language(eg. function calling)

- Code generation for expressions and statements - not done

- End-to-end compilation to a runnable executable - not done

