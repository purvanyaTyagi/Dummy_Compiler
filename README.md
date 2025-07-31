# C-Like Compiler Project(SOC 2025)

- This is my project for the Seasons of Code 2025.
- A toy compiler for a C-like programming language built entirely using C++
- The compiler supports Variable, Function declarations, and initialisation. The compiler currently only supports the int datatype.
- Scope tracking is supported along with features like if-else statements and while loops.
- At the current stage, the implementation includes a lexer that can create tokens from the input code
- A parser that generates a Parse Syntax tree and prints it to the console, and checks for syntax errors
- An IR generator class that automatically generates an IR representation of the input code.
- A semantic analyser that analyses the parsed code and looks for logical errors.

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
This will automatically build the project and run it on the code provided in input.txt

You should see a pretty-printed syntax tree and the appropriate IR representation of the code as output, along with error messages if any errors occur.

I plan on using a self designed IR analyser or using something like LLVM IR representation.

```cpp
int x;
int main(int x, int y){
    int z;
    z = 3;
    int k;
    input(k);
    output(k + x);
    return 0;
}
```

This is a sample code that will run on the current stage of the compiler.

## What have I learned so far?

- **Tokenisation**: How to break input source code into tokens (identifiers, numbers, keywords, symbols), handling whitespace and new line characters efficiently.
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

- **Parsing Tokens to create an Abstract Syntax Tree**: This part of the project helped me solidify my concepts of C++ polymorphism and recursion. The algorithm used to generate a parse tree is largely a recursive algorithm, and i had a lot of fun implementing it.
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
This is the grammar that was used for the parse tree algorithm.
The program is essentially a list of functions and variable declarations. We loop through each of these declarations and parse them recursively.

**A Virtual ASTNode class was implemented**
```cpp
class ASTNode{
    public:
        virtual ~ASTNode() = default;
        virtual void print(int indent = 0) const = 0;
        virtual void accept(Visitor& visitor) = 0;
};
```
Multiple other classes inherited this base class. Each child class had its implementation of the print and accept functions. A default destructor was used for all.

These child classes include classes like, While node, If node, variable declaration node etc.

The nodes also implement a print function that is usefull in pretty printing the Abstract syntax tree.

Here is an example,
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

This is the generated parse tree for the following code,

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

- **Smart pointers**: All object were declared as Unique_ptrs which is a smart_pointer provided by the c++ standard library. This is efficient as unique_ptrs avoid multiple pointers sharing a memory address. This avoids hogging memory, a smart pointer automatically calls destructors of the objects and frees memory when the object is no longer is in use. I learned a lot about how smart pointers work and move semantics in c++ while working on this project.

- **Semantic Analysis**: The ASTNode classes have a function called Accept, which takes in a reference to an object of type Visitor. The visitor class was implemented like this.

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

- **Symbol table generation**: To track variable declarations and initialization, we create a child of the visitor class and use algorithms that create a symbol table for this task. 

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

A vector<> was used, which kept a scope object. This vector was used like a stack to keep track of variable scopes and ensure that no two variables of the same name were declared in the same scope.

-**Three Adress Code Representation**: TAC is an intermediate representation of three-address code utilized by compilers to ease the process of code generation. Complex expressions are, therefore, decomposed into simple steps comprising, at most, three addresses: two operands and one result using this code.

I used TAC to create an Intermediate Representation of the code. This was done by creating an IRGenerator class, which inherits from the Visitor class. 

```cpp
enum class OpCode {
    ADD, SUB, MUL, DIV, MOD,
    EQ, NE, LT, LE, GT, GE,
    AND, OR, NOT,
    ASSIGN, COPY,
    GOTO, IF_FALSE, IF_TRUE,
    PARAM, CALL, RETURN,
    ARRAY_ACCESS, ARRAY_ASSIGN,
    LABEL, FUNCTION_BEGIN, FUNCTION_END
};

class IRInstruction {
public:
    OpCode op;
    std::string result;
    std::string arg1;
    std::string arg2;
    
    std::string to_string() const;
};

class IRGenerator : public Visitor {
    std::vector<IRInstruction> instructions;
    int temp_counter;
    int label_counter;
public:
    symbol_table symb_table;
    std::string new_temp();
    std::string new_label();
    void emit(OpCode op, std::string result = "", std::string arg1 = "", std::string arg2 = "");
    void print();
    void generate_instructions(const std::vector<std::unique_ptr<ASTNode>>& parsed_tree); 
    void visit(Var_Decal_Node* node) override;
    void visit(Var_Arr_Decal_Node* node) override;
    void visit(Func_Decal_Node* node) override;
    void visit(Var_Initialise_Node* node) override;
    void visit(While_Node* node) override;
    void visit(Else_Node* node) override;
    void visit(If_Node* node) override;
    void visit(Number_Expr* node) override;
    void visit(Variable_Expr* node) override;
    void visit(Binary_Expr* node) override;
    void visit(Assignment_Expr* node) override;
    void visit(Rel_Operation* node) override;
    void visit(return_node* node) override;
    void visit(input_node* node) override;
    void visit(output_node* node) override;
    void visit(Func_Call_Node* node) override;
};

```
The IRinstruction class is used to store the actual TAC instructions and IRgenerator class uses a vector of type 'IRinstruction' to create the IR code.

This was a fun process to learn and i plan to create representations of LLVM IR. to further compile this code into machine language.

here is an example of the generated IR.

```cpp
int main(int y){
    int z;
    z = 5 + y;
    int k;
    if(z > 4){
        if(z > 1){
            k = 2;
        }else{
            k = 3;
        }
    }else{
        k = 5;
    }

    int l;
    int m; 
    input(l);
    input(m); 
    output(l + m*l + m*4 + (3+(l*3)));
    return 5 + l;
}

```

```
function main begin
t1 = 5
t2 = y
t0 = t1 + t2
z = t0
t4 = z
t5 = 4
t3 = t4 > t5
ifFalse t3 goto L1
t7 = z
t8 = 1
t6 = t7 > t8
ifFalse t6 goto L3
t9 = 2
k = t9
goto L2
L3:
t10 = 3
k = t10
goto L2
L2:
goto L0
L1:
t11 = 5
k = t11
goto L0
L0:
l = call input, 
m = call input, 
t15 = l
t17 = m
t18 = l
t16 = t17 * t18
t14 = t15 + t16
t20 = m
t21 = 4
t19 = t20 * t21
t13 = t14 + t19
t23 = 3
t25 = l
t26 = 3
t24 = t25 * t26
t22 = t23 + t24
t12 = t13 + t22
output = call t12, 
t28 = 5
t29 = l
t27 = t28 + t29
return t27
function main end
```

## Current status of the project

- Lexing and tokenization - done

- Parsing into AST - done

- Symbol table implementation - done

- Semantic analysis - done

- IR code representation - done

- End-to-end compilation to a runnable executable - not done

