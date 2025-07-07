#pragma once
#include "lexer.h"
#include<iostream>
#include<vector>
#include<memory>
#include<unordered_map>
#include "parser.h"

class ASTNode;
class symbol_table;
struct SymbolInfo;

using Scope = std::unordered_map<std::string, SymbolInfo>;

class Var_Decal_Node;
class Var_Arr_Decal_Node;
class Func_Decal_Node;
class Var_Initialise_Node;
class While_Node;
class Else_Node;
class If_Node;
class Number_Expr;
class Variable_Expr;
class Binary_Expr;
class Assignment_Expr;
class Rel_Operation;
class return_node;
class output_node;
class input_node;
class Func_Call_Node;

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
        virtual void visit(Func_Call_Node* node) = 0;
        virtual ~Visitor() = default;
};

class Semantic_Analyser : public Visitor {
    private:
        symbol_table SymbolTab;
    public:
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
        void Analyse_code(const std::vector<std::unique_ptr<ASTNode>> &parsed_code);
        bool check_return_statement(std::vector<std::unique_ptr<ASTNode>>* body);
};
