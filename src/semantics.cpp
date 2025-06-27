#include<iostream>
#include<vector>
#include<memory>
#include<unordered_map>
#include "lexer.h"
#include "parser.h"
#include "semantics.h"

void Semantic_Analyser::Analyse_code(std::vector<std::unique_ptr<ASTNode>> parsed_code){
    for(int i = 0; i < parsed_code.size(); i++){
        if(dynamic_cast<Else_Node*>(parsed_code[i].get())){
            if(i >= 1){
                if(!dynamic_cast<If_Node*>(parsed_code[i - 1].get())){
                    throw std::runtime_error("Else block requires a Prior If Block");
                }
            }else{
                throw std::runtime_error("Else block requires a Prior If block");
            }
        }
        parsed_code[i]->accept(*this);
    }
}

void Semantic_Analyser::visit(Var_Decal_Node* node){
    SymbolInfo info;
    info.kind = SymbolKind::VARIABLE;
    info.type = node->type;
    info.name = node->name;
    if(!SymbolTab.declare(node->name, info)){
        throw std::runtime_error("Variable declaration failed " + node->name);
    }
}

void Semantic_Analyser::visit(Var_Arr_Decal_Node* node){
    SymbolInfo info;
    info.kind = SymbolKind::VARIABLE_ARRAY;
    info.type = node->type;
    info.name = node->name;
    if(!SymbolTab.declare(node->name, info)){
        throw std::runtime_error("Variable declaration failed " + node->name);
    };
}

void Semantic_Analyser::visit(Func_Decal_Node* node){
    if(!(node->return_type == "void" || node->return_type == "int")){
        throw std::runtime_error("Not a valid return type");
    }
    SymbolInfo info;
    info.kind = SymbolKind::FUNCTION;
    info.type = node->return_type;
    info.returnType = node->return_type;
    info.name = node->name;

    std::string temp;
    for(int j = 0; j < node->params.size(); j++){
        if((node->params)[j].second == temp){
            throw std::runtime_error("Function Parameters have the same name");
        }
        else{
            temp = (node->params)[j].second;
        }
    }
    std::vector<SymbolInfo> param_decal;
    for(auto itr = node->params.rbegin(); itr != node->params.rend(); ++itr){
        info.param_types.push_back((itr->first));
        SymbolInfo info;
        info.kind = SymbolKind::VARIABLE;
        info.type = itr->first;
        info.name = itr->second;
        param_decal.push_back(info);
    }
    if(!SymbolTab.declare(node->name, info)){
        throw std::runtime_error("Function declaration error, overloading not allowed " + node->name);
    };
    Scope Current_scope = SymbolTab.return_symbol_table().back();
    SymbolTab.enter_scope();
    for(auto itr = param_decal.begin(); itr != param_decal.end(); ++itr){
        SymbolTab.declare(itr->name, *itr);
    }
    for(auto itr = Current_scope.begin(); itr != Current_scope.end(); ++itr){
        SymbolTab.declare(itr->first, itr->second);
    }
    Analyse_code(std::move(node->body));
    SymbolTab.exit_scope();
}

void Semantic_Analyser::visit(Var_Initialise_Node* node){
    if(!SymbolTab.isDeclaredInCurrentScope(node->name_of_var)){
        throw std::runtime_error("Variable has not be declared yet");
    }
    SymbolTab.initialise(node->name_of_var);
}

void Semantic_Analyser::visit(Assignment_Expr* node){
    if(!SymbolTab.isDeclaredInCurrentScope(node->name)){
        throw std::runtime_error("No Such Variable declared");
    }
    SymbolTab.initialise(node->name);
}

void Semantic_Analyser::visit(While_Node* node){
    Scope Current_scope = SymbolTab.return_symbol_table().back();
    SymbolTab.enter_scope();
    for(auto itr = Current_scope.begin(); itr != Current_scope.end(); ++itr){
        SymbolTab.declare(itr->first, itr->second);
    }
    Analyse_code(std::move(node->body));
    SymbolTab.exit_scope();
}
void Semantic_Analyser::visit(If_Node* node){
    Scope Current_scope = SymbolTab.return_symbol_table().back();
    SymbolTab.enter_scope();
    for(auto itr = Current_scope.begin(); itr != Current_scope.end(); ++itr){
        SymbolTab.declare(itr->first, itr->second);
    }
    Analyse_code(std::move(node->body));
    SymbolTab.exit_scope();
}

void Semantic_Analyser::visit(Else_Node* node){
    Scope Current_scope = SymbolTab.return_symbol_table().back();
    SymbolTab.enter_scope();
    for(auto itr = Current_scope.begin(); itr != Current_scope.end(); ++itr){
        SymbolTab.declare(itr->first, itr->second);
    }
    Analyse_code(std::move(node->body));
    SymbolTab.exit_scope();
}

void Semantic_Analyser::visit(Number_Expr* node){ 
}

void Semantic_Analyser::visit(Variable_Expr* node){
    if(!SymbolTab.isDeclaredInCurrentScope(node->name)){
        throw std::runtime_error("Variable has not been declared");
    }
    else if(!SymbolTab.isInitialised(node->name)){
        throw std::runtime_error("Variable has not been initialised");
    }
    // if(!SymbolTab.isDeclaredInCurrentScope(node->name)){
    //     throw std::runtime_error("No Such Variable declared");
    // }
}

void Semantic_Analyser::visit(Binary_Expr* node){
    node->left->accept(*this);
    node->right->accept(*this);
}

void Semantic_Analyser::visit(Rel_Operation* node){
    node->left_expression->accept(*this);
    node->right_expression->accept(*this);
}

void Semantic_Analyser::visit(output_node* node){
    if(!(dynamic_cast<Binary_Expr*>((node->output_val).get()) || dynamic_cast<Number_Expr*>((node->output_val).get()) || dynamic_cast<Variable_Expr*>((node->output_val).get())
        || dynamic_cast<Rel_Operation*>((node->output_val).get()))){
        throw std::runtime_error("Invalid expression in Output");
    }
    node->output_val->accept(*this);
}

void Semantic_Analyser::visit(input_node* node){
    if(!dynamic_cast<Variable_Expr*>((node->input_val).get())){
        throw std::runtime_error("Only Variables allowed in input expression"); 
    }
}

void Semantic_Analyser::visit(return_node* node){
    if(!(dynamic_cast<Number_Expr*>((node->expression).get()) || dynamic_cast<Variable_Expr*>((node->expression).get()))){
            throw std::runtime_error("Invalid expression in return statement");
        }
        node->expression->accept(*this);
        //implement return type checking
}




