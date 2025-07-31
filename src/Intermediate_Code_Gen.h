#include<iostream>
#include<vector>
#include "parser.h"
#include "semantics.h"
#include "lexer.h"
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
