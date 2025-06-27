#include<iostream>
#include<memory>
#include "parser.h"
#include "lexer.h"

// void Semantic_Analyser::visit(const Var_Decal_Node& var){
// }

void symbol_table::enter_scope(){
    scopes.push_back(Scope{});
    initialised_variables.push_back(std::unordered_map<std::string, bool>{});
    //current_scope += 1;
}

void symbol_table::exit_scope(){
    scopes.pop_back();
    initialised_variables.pop_back();
}

bool symbol_table::declare(const std::string& name, const SymbolInfo& info){
    Scope& current = scopes.back();
    if(current.find(name) != current.end()){
        return false;
    }else{
        current.insert({name, info});
        return true;
    }
}

SymbolInfo* symbol_table::lookup(const std::string& name){
    for(auto it = scopes.rbegin(); it != scopes.rend(); ++it){
        if(it->count(name)) return &((*it)[name]);
    }
    return nullptr;
}

bool symbol_table::isInitialised(const std::string& name){
    return initialised_variables.back().count(name) > 0;
}

bool symbol_table::isDeclaredInCurrentScope(const std::string& name) {
    return scopes.back().count(name) > 0;
}


bool symbol_table::initialise(const std::string& name){
    std::unordered_map<std::string, bool>& current = initialised_variables.back();
    if(!isDeclaredInCurrentScope(name)){
        throw std::runtime_error("Variable has not been declared yet");
    }else{
        current.insert({name, true});
        return true;
    }
}

std::vector<Scope> symbol_table::return_symbol_table(){
    return scopes;
}


void symbol_table::contruct_symbol_table(const std::vector<std::unique_ptr<ASTNode>>& parsed_code){ //fix!! not working as expected
    for(auto it = parsed_code.rbegin(); it != parsed_code.rend(); ++it){
        if(Var_Decal_Node* var_node = dynamic_cast<Var_Decal_Node*>((*it).get())){
            SymbolInfo info;
            info.kind = SymbolKind::VARIABLE;
            info.type = var_node->type;
            info.name = var_node->name;
            if(!declare(var_node->name, info)){
                throw std::runtime_error("Variable declaration failed " + var_node->name);
            }
        }else if(Var_Arr_Decal_Node* var_arr_node = dynamic_cast<Var_Arr_Decal_Node*>((*it).get())){
            SymbolInfo info;
            info.kind = SymbolKind::VARIABLE_ARRAY;
            info.type = var_arr_node->type;
            info.name = var_arr_node->name;
            if(!declare(var_arr_node->name, info)){
                throw std::runtime_error("Variable declaration failed " + var_arr_node->name);
            };
            //info.size  = var_arr_node->size;
            //take care of array size later. //deal with this later
        }else if(Assignment_Expr* assignment_node = dynamic_cast<Assignment_Expr*>((*it).get())){
            initialise(assignment_node->name);
        }
        else if(Var_Initialise_Node* assignment_node = dynamic_cast<Var_Initialise_Node*>((*it).get())){
            initialise(assignment_node->name_of_var);
        }
        else if(Func_Decal_Node* func_node = dynamic_cast<Func_Decal_Node*>((*it).get())){
            SymbolInfo info;
            info.kind = SymbolKind::FUNCTION;
            info.type = func_node->return_type;
            info.name = func_node->name;
            for(auto itr = func_node->params.rbegin(); itr != func_node->params.rend(); ++itr){
                info.param_types.push_back((itr->first));
            }
            info.returnType = func_node->return_type;
            if(!declare(func_node->name, info)){
                throw std::runtime_error("Function declaration error, overloading not allowed " + func_node->name);
            };
            enter_scope();
            contruct_symbol_table(func_node->body); //start from here taking a break
            if (scopes.size() >= 2) {
                auto& secondLast = scopes[scopes.size() - 2];
                secondLast[func_node->name].scope = std::make_shared<Scope>(scopes.back());
            }
            exit_scope();
        }
    }
    // if(current_scope != 1){
    //     exit_scope();
    // }else{
    //     current_scope = 1;
    // }   
}


void symbol_table::print() {
    std::cout << "=== Symbol Table ===" << std::endl;
    // Reverse iterate through the scopes (from innermost to outermost)
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        std::cout << "--- Scope ---" << std::endl;

        for (const auto& pair : *it) {
            const std::string& name = pair.first;
            const SymbolInfo& info = pair.second;

            std::cout << "Name: " << name << "\n";
            std::cout << "Type: " << info.type << "\n";
            if(info.kind == SymbolKind::VARIABLE){
            std::cout << " Kind: " << "variable" << "\n";
            }else if(info.kind == SymbolKind::VARIABLE_ARRAY){
            std::cout << " Kind: " << "variable_array" << "\n";
            }else if(info.kind == SymbolKind::FUNCTION){
            std::cout << " Kind: " << "function" << "\n";
            }
            // Optional: print parameter types if it's a function
            if (info.kind == SymbolKind::FUNCTION) {
                std::cout << "  Param Types: ";
                for (const auto& t : info.param_types) {
                    std::cout << t << " ";
                }
                std::cout << "\n";
                std::cout << "-------Function scope--------" << std::endl;

                for(const auto& pair : *(info.scope)){
                    const std::string& name = pair.first;
                    const SymbolInfo& info = pair.second;
        
                    std::cout << "Name: " << name << "\n";
                    std::cout << "Type: " << info.type << "\n";
                    if(info.kind == SymbolKind::VARIABLE){
                    std::cout << " Kind: " << "variable" << "\n";
                    }else if(info.kind == SymbolKind::VARIABLE_ARRAY){
                    std::cout << " Kind: " << "variable_array" << "\n";
                    }else if(info.kind == SymbolKind::FUNCTION){
                    std::cout << " Kind: " << "function" << "\n";
                    }
                }

                std::cout << " " << "\n";
                std::cout << " " << "\n";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "=== End of Symbol Table ===" << std::endl;
}




