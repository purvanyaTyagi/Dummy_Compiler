#pragma once
#include<iostream>
#include<vector>
#include "lexer.h"
#include "parser.h"
#include "semantics.h"
#include "Intermediate_Code_Gen.h"


void IRGenerator::emit(OpCode op, std::string result, std::string arg1, std::string arg2){
    IRInstruction instruction;
    instruction.op = op;
    instruction.result = result;
    instruction.arg1 = arg1;
    instruction.arg2 = arg2;
    instructions.push_back(instruction);
}

std::string IRGenerator::new_temp() {
    return "t" + std::to_string(temp_counter++);
}

std::string IRGenerator::new_label(){
    return "L" + std::to_string(label_counter++);
}

void IRGenerator::visit(Var_Decal_Node* node){
    SymbolInfo info;
    info.kind = SymbolKind::VARIABLE;
    info.type = node->type;
    info.name = node->name;
    if(!symb_table.declare(node->name, info)){
        throw std::runtime_error("Variable declaration failed, another variable with the same name exists in the same scope " + node->name);
    }
}

void IRGenerator::visit(Var_Arr_Decal_Node* node){
    SymbolInfo info;
    info.kind = SymbolKind::VARIABLE_ARRAY;
    info.type = node->type;
    info.name = node->name;
    if(!symb_table.declare(node->name, info)){
        throw std::runtime_error("Variable declaration failed, another variable with the same name exists in the same scope " + node->name);
    }
}

void IRGenerator::visit(Func_Decal_Node* node){
    emit(OpCode::FUNCTION_BEGIN, node->name);
    for(auto itr = node->body.begin(); itr != node->body.end(); ++itr){
        itr->get()->accept(*this); 
    }
    emit(OpCode::FUNCTION_END, node->name);
}

void IRGenerator::visit(Var_Initialise_Node* node){
}

void IRGenerator::visit(Assignment_Expr* node){
    std::string current_temp = "t" + std::to_string(temp_counter);
    node->value->accept(*this);
    emit(OpCode::ASSIGN, node->name, current_temp);
}

//temp_counter = 3, current_temp = t0, left_temp = t1, right_temp = t2
//current_temp = t1, t1 = x;
//current_temp = t2, t2 = y;

void IRGenerator::visit(Binary_Expr* node){
    std::string current_temp = new_temp();

    std::string left_temp = "t" + std::to_string(temp_counter);
    node->left->accept(*this);

    std::string right_temp = "t" + std::to_string(temp_counter);
    node->right->accept(*this);

    if(node->op == "+"){
        emit(OpCode::ADD, current_temp, left_temp, right_temp);
    }else if(node->op == "-"){
        emit(OpCode::SUB, current_temp, left_temp, right_temp);
    }else if(node->op == "*"){
        emit(OpCode::MUL, current_temp, left_temp, right_temp);
    }else if(node->op == "/"){
        emit(OpCode::DIV, current_temp, left_temp, right_temp);
    }
}

void IRGenerator::visit(Variable_Expr* node){
    std::string current_temp = new_temp();
    emit(OpCode::ASSIGN, current_temp, node->name);
}

void IRGenerator::visit(Number_Expr* node){
    std::string current_temp = new_temp();
    emit(OpCode::ASSIGN, current_temp, std::to_string(node->value));
}

void IRGenerator::visit(While_Node* node){
    std::string start_label = new_label();
    std::string end_label = new_label();
    emit(OpCode::LABEL, start_label);
    for(auto itr = node->body.begin(); itr != node->body.end(); ++itr){
        (*itr)->accept(*this);
    }
    std::string current_condition_temp = "t" + std::to_string(temp_counter);
    node->expression->accept(*this);
    emit(OpCode::IF_TRUE, current_condition_temp);
    emit(OpCode::GOTO, start_label);
    emit(OpCode::LABEL, end_label);
}

void IRGenerator::visit(If_Node* node){
    std::string current_condition_temp = "t" + std::to_string(temp_counter);
    node->expression->accept(*this);
    std::string end_label = new_label();
    std::string else_label = new_label();

    if(node->does_else_exist){
        emit(OpCode::IF_FALSE, else_label, current_condition_temp);
    }else{
        emit(OpCode::IF_FALSE, end_label, current_condition_temp);
    }
    //emit(OpCode::GOTO, end_label);
    for(auto& stmt : node->body){
        stmt->accept(*this);
    }
    emit(OpCode::GOTO, end_label);
    // emit(OpCode::GOTO, end_label);
    // emit(OpCode::LABEL, end_label);
    if(node->does_else_exist){
        emit(OpCode::LABEL, else_label);
        for(auto& stmt : dynamic_cast<Else_Node*>(node->else_node.get())->body){
            stmt->accept(*this);   
        }
        emit(OpCode::GOTO, end_label);
        emit(OpCode::LABEL, end_label);
    }else{
        emit(OpCode::GOTO, end_label);
        emit(OpCode::LABEL, end_label);
    }
}

void IRGenerator::visit(Else_Node* node){
    // for(auto& stmt : node->body){
    //     stmt->accept(*this);
    // }
}

void IRGenerator::visit(Rel_Operation* node){
    std::string current_temp = new_temp();

    std::string left_temp = "t" + std::to_string(temp_counter);
    node->left_expression->accept(*this);

    std::string right_temp = "t" + std::to_string(temp_counter);
    node->right_expression->accept(*this);

    if(node->op == "=="){
        emit(OpCode::EQ, current_temp, left_temp, right_temp);
    }else if(node->op == "!="){
        emit(OpCode::NE, current_temp, left_temp, right_temp);
    }else if(node->op == "<"){
        emit(OpCode::LT, current_temp, left_temp, right_temp);
    }else if(node->op == ">"){
        emit(OpCode::GT, current_temp, left_temp, right_temp);
    }else if(node->op == "<="){
        emit(OpCode::LE, current_temp, left_temp, right_temp);
    }else if(node->op == ">="){
        emit(OpCode::GE, current_temp, left_temp, right_temp);
    }
}

void IRGenerator::visit(return_node* node){
    std::string current_temp = "t" + std::to_string(temp_counter);
    node->expression->accept(*this);

    emit(OpCode::RETURN, current_temp);
}

void IRGenerator::visit(input_node* node){
    Variable_Expr* variable_node = dynamic_cast<Variable_Expr*>((node->input_val).get());
    emit(OpCode::CALL, variable_node->name, "input");
}

void IRGenerator::visit(output_node* node){
    std::string current_temp = "t" + std::to_string(temp_counter);
    node->output_val->accept(*this);
    emit(OpCode::CALL, "output", (current_temp));
}

void IRGenerator::visit(Func_Call_Node* node){
    emit(OpCode::CALL, node->name);
    for(auto& itr : node->params){
        std::string current_temp = "t" + std::to_string(temp_counter);
        itr->accept(*this);
        emit(OpCode::PARAM, node->name, current_temp); 
    }
}

std::string IRInstruction::to_string() const {
    switch (op) {
        case OpCode::ADD:       return result + " = " + arg1 + " + " + arg2;
        case OpCode::SUB:       return result + " = " + arg1 + " - " + arg2;
        case OpCode::MUL:       return result + " = " + arg1 + " * " + arg2;
        case OpCode::DIV:       return result + " = " + arg1 + " / " + arg2;
        case OpCode::MOD:       return result + " = " + arg1 + " % " + arg2;

        case OpCode::EQ:        return result + " = " + arg1 + " == " + arg2;
        case OpCode::NE:        return result + " = " + arg1 + " != " + arg2;
        case OpCode::LT:        return result + " = " + arg1 + " < " + arg2;
        case OpCode::LE:        return result + " = " + arg1 + " <= " + arg2;
        case OpCode::GT:        return result + " = " + arg1 + " > " + arg2;
        case OpCode::GE:        return result + " = " + arg1 + " >= " + arg2;

        case OpCode::AND:       return result + " = " + arg1 + " && " + arg2;
        case OpCode::OR:        return result + " = " + arg1 + " || " + arg2;
        case OpCode::NOT:       return result + " = !" + arg1;

        case OpCode::ASSIGN:    return result + " = " + arg1;
        case OpCode::COPY:      return result + " := " + arg1;

        case OpCode::GOTO:      return "goto " + result;
        case OpCode::IF_FALSE:  return "ifFalse " + arg1 + " goto " + result;
        case OpCode::IF_TRUE:   return "if " + arg1 + " goto " + result;

        case OpCode::PARAM:     return "param " + result;
        case OpCode::CALL:      return result + " = call " + arg1 + ", " + arg2;
        case OpCode::RETURN:    return "return " + result;

        case OpCode::ARRAY_ACCESS:  return result + " = " + arg1 + "[" + arg2 + "]";
        case OpCode::ARRAY_ASSIGN:  return result + "[" + arg1 + "] = " + arg2;

        case OpCode::LABEL:         return result + ":";
        case OpCode::FUNCTION_BEGIN:return "function " + result + " begin";
        case OpCode::FUNCTION_END:  return "function " + result + " end";

        default: return "Unknown OpCode";
    }
}

void IRGenerator::print(){
    for(auto& itr : instructions){
        std::cout << itr.to_string() << std::endl;
    }
}

void IRGenerator::generate_instructions(const std::vector<std::unique_ptr<ASTNode>>& parsed_tree){
    temp_counter = 0;
    label_counter = 0;

    for(auto itr = parsed_tree.begin(); itr != parsed_tree.end(); ++itr){
        (*itr)->accept(*this);
    }
    print();
}

//fix if else code gen tom;

