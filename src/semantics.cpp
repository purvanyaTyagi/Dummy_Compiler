#include<iostream>
#include<vector>
#include<memory>
#include<unordered_map>
#include "lexer.h"
#include "parser.h"
#include "semantics.h"

bool Semantic_Analyser::check_return_statement(std::vector<std::unique_ptr<ASTNode>>* body){
    for(auto itr = body->begin(); itr != body->end(); ++itr){
        if(dynamic_cast<return_node*>(itr->get())){
            return true;
        }else if (auto if_node = dynamic_cast<If_Node*>(itr->get())) {
            if((itr + 1) != body->end()){
                if (auto else_node = dynamic_cast<Else_Node*>((itr + 1)->get())) {
                    if(check_return_statement(&(if_node->body)) && check_return_statement(&(else_node->body))){
                        return true;
                    }else{
                        continue;
                    }
                }           
            }
            // if(check_return_statement(&(if_node->body))){
            //     continue;
            // }else{
            //     return false;
            // }
        }else if(auto while_node = dynamic_cast<While_Node*>(itr->get())){
            if(check_return_statement(&(while_node->body))){
                return true;
            }else{
                continue;
            }
        }
    }
    return false;
}

void Semantic_Analyser::Analyse_code(const std::vector<std::unique_ptr<ASTNode>>& parsed_code){
    for(long unsigned int i = 0; i < parsed_code.size(); i++){
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
    const auto& symbol_table_stack = SymbolTab.return_symbol_table();
    bool isDeclaredinPreviousScope = false;
    if (!symbol_table_stack.empty()) {
        for (auto it = symbol_table_stack.begin();
             it != symbol_table_stack.end() - 1;
             ++it) {
            auto& this_scope = *it;
            if (this_scope.find(node->name) != this_scope.end()) {
                isDeclaredinPreviousScope = true;
                break;
            }
        }
    }
    
    // bool isDeclaredinPreviousScope = false;
    // if (!SymbolTab.return_symbol_table().empty()){
    //     for (auto it = SymbolTab.return_symbol_table().begin(); it != SymbolTab.return_symbol_table().end() - 1; ++it) {
    //         auto this_scope = *it;
    //         if(this_scope.find(node->name) != this_scope.end()){
    //             isDeclaredinPreviousScope = true;
    //             break;
    //         }
    //     }
    // }
    SymbolInfo info;
    info.kind = SymbolKind::VARIABLE;
    info.type = node->type;
    info.name = node->name;
    if(!SymbolTab.declare(node->name, info) && !isDeclaredinPreviousScope){
        throw std::runtime_error("Variable declaration failed, another variable with the same name exists in the same scope " + node->name);
    }
    else if(!SymbolTab.declare(node->name, info) && isDeclaredinPreviousScope){
        std::cout << "Warning: Variable is already declared in previous scopes, using previously initialised value if initialised." << std::endl;
    }
}

void Semantic_Analyser::visit(Var_Arr_Decal_Node* node){
    // bool isDeclaredinPreviousScope = false;
    // if (!SymbolTab.return_symbol_table().empty()){
    //     for (auto it = SymbolTab.return_symbol_table().begin(); it != SymbolTab.return_symbol_table().end() - 1; ++it) {
    //         auto this_scope = *it;
    //         if(this_scope.find(node->name) != this_scope.end()){
    //             isDeclaredinPreviousScope = true;
    //             break;
    //         }
    //     }
    // }
    const auto& symbol_table_stack = SymbolTab.return_symbol_table();
    bool isDeclaredinPreviousScope = false;
    if (!symbol_table_stack.empty()) {
        for (auto it = symbol_table_stack.begin();
             it != symbol_table_stack.end() - 1;
             ++it) {
            auto& this_scope = *it;
            if (this_scope.find(node->name) != this_scope.end()) {
                isDeclaredinPreviousScope = true;
                break;
            }
        }
    }

    SymbolInfo info;
    info.kind = SymbolKind::VARIABLE_ARRAY;
    info.type = node->type;
    info.name = node->name;
    if(!SymbolTab.declare(node->name, info) && !isDeclaredinPreviousScope){
        throw std::runtime_error("Variable declaration failed, another variable with the same name exists in the same scope " + node->name);
    }
    else if(!SymbolTab.declare(node->name, info) && isDeclaredinPreviousScope){
        std::cout << "Warning: Variable is already declared in previous scopes, using previously initialised value of initialised." << std::endl;
    }
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
    for(long unsigned int j = 0; j < node->params.size(); j++){
        if((node->params)[j].second == temp){
            throw std::runtime_error("Function Parameters have the same name");
        }
        else{
            temp = (node->params)[j].second;
            //info.param_types.push_back("int");
        }
    }
    std::vector<SymbolInfo> param_decal;
    for(auto itr = node->params.begin(); itr != node->params.end(); ++itr){
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
    auto initilalised_variables = SymbolTab.return_initilasied_variables().back();
    SymbolTab.enter_scope();
    for(auto itr = Current_scope.begin(); itr != Current_scope.end(); ++itr){
        SymbolTab.declare(itr->first, itr->second);
    }
    for(auto itr = initilalised_variables.begin(); itr != initilalised_variables.end(); ++itr){
        SymbolTab.initialise(itr->first);
    }
    for(auto itr = param_decal.begin(); itr != param_decal.end(); ++itr){
        if(!SymbolTab.declare(itr->name, *itr)){
            throw std::runtime_error("Variable declaration failed, another variable with the same name exists in the previous scope " + itr->name);
        }
        SymbolTab.initialise(itr->name);
    }
    Analyse_code(node->body);
    if(!check_return_statement(&(node->body)) && node->return_type != "void"){
        throw std::runtime_error("Return statement not found for the function name: " + node->name);
    }else if(check_return_statement(&(node->body)) && node->return_type == "void"){
        throw std::runtime_error("Void functions do not return values.");
    }    
    // for(auto itr = node->body.begin(); itr != node->body.end(); ++itr){
    //     if(dynamic_cast<return_node*>(itr->get())){
    //         has_return = true;
    //     }
    //     else if(dynamic_cast<If_Node*>(itr->get())){

    //     }
    // }
    SymbolTab.exit_scope();
}

void Semantic_Analyser::visit(Var_Initialise_Node* node){
    if(!SymbolTab.isDeclaredInCurrentScope(node->name_of_var)){
        throw std::runtime_error("Variable has not be declared yet: " + node->name_of_var);
    }
    SymbolTab.initialise(node->name_of_var);
}

void Semantic_Analyser::visit(Assignment_Expr* node){
    if(!SymbolTab.isDeclaredInCurrentScope(node->name)){
        throw std::runtime_error("No Such Variable declared: " + node->name);
    }
    node->value->accept(*this);
    auto& symbol_table_reference = SymbolTab.return_reference_table();
    auto& initialised_table_reference = SymbolTab.return_initialised_reference_table();

    if(symbol_table_reference.size() == initialised_table_reference.size()){
        for(long unsigned int i = 0; i < symbol_table_reference.size(); i++){
            if(symbol_table_reference[i].find(node->name) != symbol_table_reference[i].end()){
                initialised_table_reference[i][node->name] = true;
            }
        }
    }
    //SymbolTab.initialise(node->name);
}

void Semantic_Analyser::visit(While_Node* node){
    node->expression->accept(*this);
    Scope Current_scope = SymbolTab.return_symbol_table().back();
    auto initilalised_variables = SymbolTab.return_initilasied_variables().back();
    SymbolTab.enter_scope();
    for(auto itr = Current_scope.begin(); itr != Current_scope.end(); ++itr){
        SymbolTab.declare(itr->first, itr->second);
    }
    for(auto itr = initilalised_variables.begin(); itr != initilalised_variables.end(); ++itr){
        SymbolTab.initialise(itr->first);
    }
    Analyse_code(node->body);
    SymbolTab.exit_scope();
}
void Semantic_Analyser::visit(If_Node* node){
    node->expression->accept(*this);
    Scope Current_scope = SymbolTab.return_symbol_table().back();
    auto initilalised_variables = SymbolTab.return_initilasied_variables().back();
    SymbolTab.enter_scope();
    for(auto itr = Current_scope.begin(); itr != Current_scope.end(); ++itr){
        SymbolTab.declare(itr->first, itr->second);
    }
    for(auto itr = initilalised_variables.begin(); itr != initilalised_variables.end(); ++itr){
        SymbolTab.initialise(itr->first);
    }
    Analyse_code(node->body);

    // auto& symbol_table_reference = SymbolTab.return_reference_table();
    // auto& initialised_table_reference = SymbolTab.return_initialised_reference_table();
    // auto size = initialised_table_reference.size();

    // for(long unsigned int i = 0; i < symbol_table_reference.size() - 1; i++){
    //     for(const auto& pair : symbol_table_reference[i]){
    //         if(SymbolTab.isInitialised(pair.first)){
    //             for(long unsigned int j = 0; j < size; j++){
    //                 initialised_table_reference[j].erase(pair.first);
    //             }
    //         }
    //     }
    // }
    SymbolTab.exit_scope();
}

void Semantic_Analyser::visit(Func_Call_Node* node){
    auto& scopes = SymbolTab.return_reference_table();
    if(scopes[0].find(node->name) != scopes[0].end()){
        SymbolInfo func_info = scopes[0][node->name];
        if(node->params.size() != func_info.param_types.size()){
            throw std::runtime_error("Too many or too few arguments for function: " + node->name);  
        }
        if(func_info.returnType != node->return_type){
            throw std::runtime_error("Function return type does not match its declaration: " + node->name);
        }
    }else{
        throw std::runtime_error("The function is not declared, name: " + node->name);          
    }
}

void Semantic_Analyser::visit(Else_Node* node){
    Scope Current_scope = SymbolTab.return_symbol_table().back();
    auto initilalised_variables = SymbolTab.return_initilasied_variables().back();
    SymbolTab.enter_scope();
    for(auto itr = Current_scope.begin(); itr != Current_scope.end(); ++itr){
        SymbolTab.declare(itr->first, itr->second);
    }
    for(auto itr = initilalised_variables.begin(); itr != initilalised_variables.end(); ++itr){
        SymbolTab.initialise(itr->first);
    }
    Analyse_code(node->body);
    SymbolTab.exit_scope();
}

void Semantic_Analyser::visit(Number_Expr* node){ 
}

void Semantic_Analyser::visit(Variable_Expr* node){
    if(!SymbolTab.isDeclaredInCurrentScope(node->name)){
        throw std::runtime_error("Variable has not been declared yet: " + node->name);
    }
    else if(!SymbolTab.isInitialised(node->name)){
        throw std::runtime_error("Variable has not been initialised: " + node->name);
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
        throw std::runtime_error("Invalid expression in Output, only Binary, relational, variable and numerical expressions are allowed");
    }
    node->output_val->accept(*this);
}

void Semantic_Analyser::visit(input_node* node){
    if(!dynamic_cast<Variable_Expr*>((node->input_val).get())){
        throw std::runtime_error("Only Variables allowed in input expression"); 
    }
}

void Semantic_Analyser::visit(return_node* node){
    if(!(dynamic_cast<Number_Expr*>((node->expression).get()) || dynamic_cast<Binary_Expr*>((node->expression).get()) || dynamic_cast<Variable_Expr*>((node->expression).get()))){
            throw std::runtime_error("Invalid expression in return statement");
        }
        node->expression->accept(*this);
        //implement return type checking
}




