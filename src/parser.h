#pragma once
#include<iostream>
#include<memory>
#include<vector>
#include<string>
#include<stack>
#include "lexer.h"
#include <unordered_map>
// program        → declaration-list
// declaration    → var-declaration | fun-declaration   
// var-declaration → type-specifier ID ';' | type-specifier ID '[' NUM ']' ';'
// fun-declaration → type-specifier ID '(' params ')' compound-stmt
// type-specifier → 'int' | 'void'
// compound-stmt  → '{' local-declarations statement-list '}'
// statement      → expression-stmt | compound-stmt | selection-stmt | 
//                  iteration-stmt | return-stmt
// expression     → var '=' expression | simple-expression
// simple-expression → additive-expression relop additive-expression
// factor         → '(' expression ')' | var | call | NUM

class Visitor;

enum class SymbolKind{
    VARIABLE,
    VARIABLE_ARRAY,
    FUNCTION

};

struct SymbolInfo;

using Scope = std::unordered_map<std::string, SymbolInfo>;

struct SymbolInfo {
    SymbolKind kind;
    std::string type;
    std::string name;
    //std::unique_ptr<ASTNode> size; //only for variable arrays
    std::vector<std::string> param_types; //only for functions
    std::string returnType; //only for functions
    std::shared_ptr<Scope> scope; //only for functions
};
void print_ident(int indent);

class ASTNode{
    public:
        virtual ~ASTNode() = default;
        virtual void print(int indent = 0) const = 0;
        virtual void accept(Visitor& visitor) = 0;
};

class parser{
    private:
        std::vector<Token> tokens;
        std::vector<std::unique_ptr<ASTNode>> parsed_code;
        long unsigned int current = 0;
        std::stack<char> tracker;

    public:
        parser(std::vector<Token> tokens) : tokens(tokens) {
            std::cout << "Parser Initialised" << std::endl;   
        }

        void parse_program();
        void print_tree();
        void expect(std::string expected_token, const std::string& error_message, int current_position);
        std::vector<std::unique_ptr<ASTNode>> return_parsed_code();
        std::vector<std::unique_ptr<ASTNode>> parse_compound_statement();
        std::unique_ptr<ASTNode> parse_local_declarations();
        std::vector<std::unique_ptr<ASTNode>> parse_statement_list();
        std::unique_ptr<ASTNode> parse_variable_declaration();
        std::unique_ptr<ASTNode> parse_function_declaration();
        std::unique_ptr<ASTNode> parse_simple_expression();
        std::unique_ptr<ASTNode> parse_additive_expression();
        std::unique_ptr<ASTNode> parse_term();
        std::unique_ptr<ASTNode> parseFactor();
        std::unique_ptr<ASTNode> parse_relational_expression();
        std::unique_ptr<ASTNode> parse_statement();


        void error_recovery();
        void synchronize();
};

class Var_Decal_Node : public ASTNode {
    public:
        std::string type;
        std::string name;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Var_Decal_Node(std::string type, std::string name) : type(type), name(name) {}

};

class Var_Arr_Decal_Node : public ASTNode {
    public:
        std::string type;
        std::string name;
        std::unique_ptr<ASTNode> size;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Var_Arr_Decal_Node(std::string type, std::string name, std::unique_ptr<ASTNode> size) : type(type), name(name), size(std::move(size)) {}
};

class Func_Decal_Node : public ASTNode {
    public:
        std::string return_type;
        std::string name;
        std::vector<std::unique_ptr<ASTNode>> body;
        std::vector<std::pair<std::string, std::string>> params;

        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Func_Decal_Node(std::string return_type, std::string name,
             std::vector<std::unique_ptr<ASTNode>> body,
              std::vector<std::pair<std::string, std::string>> params) : return_type(return_type), name(name), body(std::move(body)), params(params){}
};

class Var_Initialise_Node : public ASTNode {
    public:
        std::string name_of_var;
        std::string value_of_var;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Var_Initialise_Node(std::string name_of_var, std::string value_of_var) : name_of_var(name_of_var), value_of_var(value_of_var){}
};

class While_Node : public ASTNode {
    public:
        std::vector<std::unique_ptr<ASTNode>> body;
        std::unique_ptr<ASTNode> expression;
        void print(int indent = 0) const override; 
        void accept(Visitor& visitor) override;
        While_Node(std::vector<std::unique_ptr<ASTNode>> body, std::unique_ptr<ASTNode> expression) : body(std::move(body)), expression(std::move(expression)){} 
};

class Else_Node : public ASTNode {
    public:
        std::vector<std::unique_ptr<ASTNode>> body;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Else_Node(std::vector<std::unique_ptr<ASTNode>> body) : body(std::move(body)) {}
};

class If_Node : public ASTNode {
    public:

        std::vector<std::unique_ptr<ASTNode>> body;
        std::unique_ptr<ASTNode> expression;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        If_Node(std::vector<std::unique_ptr<ASTNode>> body, std::unique_ptr<ASTNode> expression) : body(std::move(body)), expression(std::move(expression)){}  
};

class Number_Expr : public ASTNode {
    public:
        int value;
        // void print(int indent = 0) const override {
        //     print_ident(indent);
        //     std::cout << "Number(" << value << ")\n";
        // }
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Number_Expr(int value) : value(value) {}

};

class Variable_Expr : public ASTNode {
    public:
        std::string name;
        // void print(int indent = 0) const override {
        //     print_ident(indent);
        //     std::cout << "Variable(" << name << ")\n";
        // }
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Variable_Expr(std::string name) : name(name) {}
};

class Binary_Expr : public ASTNode {
    public:
        std::unique_ptr<ASTNode> left; 
        std::string op;
        std::unique_ptr<ASTNode> right;
        // void print(int indent = 0) const override {

        // }
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Binary_Expr(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right, std::string op)
        : left(std::move(left)), op(op), right(std::move(right)) {}
};

class Assignment_Expr : public ASTNode {
    public:
        std::string name;
        std::unique_ptr<ASTNode> value;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Assignment_Expr(std::string name, std::unique_ptr<ASTNode> value) : name(name), value(std::move(value)) {}
};

class Rel_Operation : public ASTNode {
    public:
        std::unique_ptr<ASTNode> left_expression;
        std::unique_ptr<ASTNode> right_expression;
        std::string op;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        Rel_Operation(std::unique_ptr<ASTNode> left_expression, std::unique_ptr<ASTNode> right_expression, std::string op)
            : left_expression(std::move(left_expression)), right_expression(std::move(right_expression)), op(op) {}
};

class return_node : public ASTNode {
    public:
        std::unique_ptr<ASTNode> expression;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        return_node(std::unique_ptr<ASTNode> expression) : expression(std::move(expression)) {}  
};

class output_node : public ASTNode {
    public:
        std::unique_ptr<ASTNode> output_val;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        output_node(std::unique_ptr<ASTNode> output_val) : output_val(std::move(output_val)) {}   
};

class input_node : public ASTNode {
    public:
        std::unique_ptr<ASTNode> input_val;
        void print(int indent = 0) const override;
        void accept(Visitor& visitor) override;
        input_node(std::unique_ptr<ASTNode> input_val) : input_val(std::move(input_val)) {}   
};

// class Semantic_Analyser : public Visitor{
//     private:
//         symbol_table table;
//     public:
//         void visit(const Var_Decal_Node& var) override;        
// };

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
