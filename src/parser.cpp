#include<iostream>
#include<memory>
#include<vector>
#include "parser.h"
#include "lexer.h"
#include "semantics.h"
#include <typeinfo>


//we now implement syntax analysis for the language.
void print_ident(int indent){
    for(int i = 0; i < indent; ++i) std::cout << "  ";
}

void parser::print_tree(){
    for(int i = 0; i < parsed_code.size(); ++i){
        parsed_code[i]->print(0); 
    }
}
void parser::parse_program(){
    while(current < tokens.size()){
        if(tokens[current].value == "int" || tokens[current].value == "void"){
            if(tokens[current + 2].value == std::string(1, '(')){
                parsed_code.push_back(std::move(parse_function_declaration()));

            }else{
                parsed_code.push_back(std::move(parse_variable_declaration()));
            }
        }else{
            throw std::runtime_error("Unexpected token in parse_program: " + tokens[current].value + " at line " + std::to_string(tokens[current].line) + ", column " + std::to_string(tokens[current].column));
        }
    }
}

std::vector<std::unique_ptr<ASTNode>> parser::return_parsed_code(){
    return std::move(parsed_code);
}

std::unique_ptr<ASTNode> parser::parse_variable_declaration(){
    if(tokens[current + 2].value == std::string(1, '[')){
        std::string type = tokens[current].value;
        std::string name = tokens[current + 1].value;
        current = current + 3; //current now points at the first token after the opening square bracket
        auto expr = parse_simple_expression();
        expect("]", "Expected closing square bracket after variable array declaration", current); //implement later to check if expr is a number_expr only;
        current += 1; //assuming the next token is a closing square bracket
        expect(";", "Expected semicolon after variable array declaration", current);
        current += 1; //assuming the next token is a semicolon
        return std::make_unique<Var_Arr_Decal_Node>(type, name, std::move(expr));
    }else{
        std::string type = tokens[current].value;
        std::string name = tokens[current + 1].value;
        current = current + 2;
        expect(";", "Expected semicolon after variable declaration", current);
        current = current + 1;
        return std::make_unique<Var_Decal_Node>(type, name);
    }
}

std::unique_ptr<ASTNode> parser::parse_function_declaration(){
    std::string return_type = tokens[current].value;
    std::string name = tokens[current + 1].value;
    expect("(", "Expected opening parenthesis after function name", current + 2);
    std::vector<std::unique_ptr<ASTNode>> body;
    std::vector<std::pair<std::string, std::string>> params;
    int counter = current + 3;
    // while(true){  //update if commas are counted as tokens and a part of code.
    //     if(tokens[counter].value == "int"){
    //         params.push_back(std::make_pair(tokens[counter].value, tokens[counter + 1].value));
    //         counter += 2;
    //         if(tokens[counter].value == std::string(1, ',') || tokens[counter].value == ")"){
    //             std::cout << "encountered ,";
    //             counter++;
    //             continue;
    //         }else{
    //             throw std::runtime_error("Expected closing parenthesis after function parameters");
    //         }
    //     }else if(tokens[counter].value == ")"){
    //         break;
    //     }else{
    //         std::runtime_error("Unkown token in function parameters");
    //     }
    // }
    while (tokens[counter].value != ")") {
        if (tokens[counter].value == "int") {
            if (tokens[counter + 1].type != TokenType::identifiers)
                throw std::runtime_error("Expected identifier");
    
            params.push_back(std::make_pair(tokens[counter].value, tokens[counter + 1].value));
            counter += 2;
    
            if (tokens[counter].value == ",") {
                counter++;
            }
            else if (tokens[counter].value == ")") {
                break;
            }
            else {
                throw std::runtime_error("Expected ',' or ')' after parameter");
            }
        } else {
            throw std::runtime_error("Expected type in parameter list");
        }
    }    
    //expect(")", "Expected closing parenthesis after function parameters", counter);
    current = counter + 1;
    expect("{", "Expected opening curly bracket after function parameters", current);
    current += 1;
    body = parse_compound_statement(); //begin here and implement compound statement parser.
    expect("}", "Expected closing curly bracket after function body", current);
    current += 1; //assuming the next token is a closing curly bracket
    //after parsing the compound statement, we can safely assume that the current token is the next token after the closing curly bracket.
    return std::make_unique<Func_Decal_Node>(return_type, name, std::move(body), params);
}

std::vector<std::unique_ptr<ASTNode>> parser::parse_compound_statement(){
    //currently counter points at the first token of the function after curly bracket;
    std::vector<std::unique_ptr<ASTNode>> body;
    //while(tokens[current].value != std::string(1, '}'))
    while(true){
        if(current >= tokens.size()){
            throw std::runtime_error("Reach end of token stream before compound statement declaration");
        }
        if(tokens[current].value == "int" && current < tokens.size()){
            if(tokens[current + 2].value == std::string(1, '[')){
                current += 3;
                int temp = current;
                auto expr = parse_simple_expression();
                expect("]", "Expected closing square bracket after array size assignment", current);
                expect(";", "Expected semicolon after variable declaration", current + 1);
                body.push_back(std::make_unique<Var_Arr_Decal_Node>(tokens[temp - 3].value, tokens[temp - 2].value, std::move(expr)));
                current += 2;
            }else{
                body.push_back(std::make_unique<Var_Decal_Node>(tokens[current].value, tokens[current + 1].value));
                expect(";", "Expected semicolon after variable declaration", current + 2);
                current += 3;   
            }
        }
        else if(tokens[current].value == "while" && current < tokens.size()){ //also have to implement single line while loops and if statements
            std::vector<std::unique_ptr<ASTNode>> while_body;
            std::unique_ptr<ASTNode> expression;
            current += 1;
            expect("(", "Expected while loop condition", current);
            current += 1;
            expression = parse_relational_expression();
            expect(")", "Expected closing ) bracket", current);
            current += 1; //assuming the next token is a curly bracket
            expect("{", "Expected opening curly bracket", current);
            current += 1;
            while_body = parse_compound_statement();
            current += 1; //assuming the next token is a closing curly bracket
            body.push_back(std::make_unique<While_Node>(std::move(while_body), std::move(expression)));
        }
        else if(tokens[current].value == "if" && current < tokens.size()){
            std::vector<std::unique_ptr<ASTNode>> if_body;
            std::unique_ptr<ASTNode> expression;
            current += 1;
            expect("(", "Expected if loop condition", current);
            current += 1;
            expression = parse_relational_expression();
            expect(")", "Expected closing ) bracket", current);
            current += 1; //assuming the next token is a curly bracket
            expect("{", "Expected opening curly bracket", current);
            current += 1;
            if_body = parse_compound_statement();
            current += 1; //assuming the next token is a closing curly bracket
            body.push_back(std::make_unique<If_Node>(std::move(if_body), std::move(expression)));
        }
        else if(tokens[current].value == "else" && current < tokens.size()){
            std::vector<std::unique_ptr<ASTNode>> else_body;
            current += 1;
            expect("{", "Expected opening curly bracket", current);
            current += 1;
            else_body = parse_compound_statement();
            expect("}", "Expected closing curly bracket" , current);
            current += 1; //assuming the next token is a closing curly bracket
            body.push_back(std::make_unique<Else_Node>(std::move(else_body)));
        }
        else if(tokens[current].value == "return" && current < tokens.size()){
            current += 1;
            auto expr = parse_relational_expression();
            expect(";", "Expected semicolon after return statement", current);
            current += 1; //assuming the next token is a semicolon
            //if the next token is a semicolon, we can safely assume that the expression is complete.
            //if the next token is not a semicolon, we can assume that the expression is not complete and we need to parse it further.
            body.push_back(std::make_unique<return_node>(std::move(expr)));
        }
        else if(tokens[current].type == TokenType::identifiers && tokens[current + 1].value == std::string(1, '=') && current < tokens.size()){
            auto name = tokens[current].value;
            current += 2;
            auto value = parse_relational_expression();
            expect(";", "Expected semicolon after variable initilisation", current);
            current += 1; //assuming the next token is a semicolon
            body.push_back(std::make_unique<Assignment_Expr>(name, std::move(value)));
        }
        else if(tokens[current].value == "output" && current < tokens.size()){
            current += 1;
            expect("(", "Expected ( after output token", current);
            current += 1;
            std::unique_ptr<ASTNode> output_val;
            output_val = parse_relational_expression();
            expect(")", "Expected ) after output statement decalaration", current);
            current += 1;
            expect(";", "Expected semicolon after output statement", current);
            current += 1;
            body.push_back(std::make_unique<output_node>(std::move(output_val)));   
        }else if(tokens[current].value == "}" && current < tokens.size()){
            break;
        }else{
            throw std::runtime_error("Unexpected Token in compound statement: " + tokens[current].value);
        }
    }
    return body;
}

std::unique_ptr<ASTNode> parser::parse_relational_expression(){
    auto left = parse_simple_expression();
    if(tokens[current].value == std::string(1, '<') || tokens[current].value == std::string(1, '>') || 
       tokens[current].value == std::string(1, '=') || tokens[current].value == std::string(1, '!') || 
       tokens[current].value == "<=" || tokens[current].value == ">=" || tokens[current].value == "==" ||
       tokens[current].value == "!="){
        std::string op = tokens[current].value;
        current += 1;
        auto right = parse_simple_expression();

        return std::make_unique<Rel_Operation>(std::move(left), std::move(right), op);
    }
    return left; //if no relational operator is found, return the left expression.
}

std::unique_ptr<ASTNode> parser::parse_simple_expression(){ //current points at the first token of the expression. after parsing, it will point at the next token after the expression.
    auto expr = parse_term();
    while(tokens[current].value == std::string(1, '+') || tokens[current].value == std::string(1, '-')){
        std::string op = tokens[current].value;
        current += 1;
        auto right = parse_term();
        expr = std::make_unique<Binary_Expr>(std::move(expr), std::move(right), op);
    }
    return expr;
}
std::unique_ptr<ASTNode> parser::parse_term(){
    auto expr = parseFactor();
    while(tokens[current].value == std::string(1, '*') || tokens[current].value == std::string(1, '/')){
        std::string op = tokens[current].value;
        current += 1;
        auto right = parse_term();
        expr = std::make_unique<Binary_Expr>(std::move(expr), std::move(right), op);
    }
    return expr;
}

std::unique_ptr<ASTNode> parser::parseFactor(){
    if(tokens[current].type == TokenType::numbers){
        std::string value = tokens[current].value;
        current += 1;
        return std::make_unique<Number_Expr>(std::stoi(value));
    }else if(tokens[current].type == TokenType::identifiers){
        std::string name = tokens[current].value;   
        current += 1;
        return std::make_unique<Variable_Expr>(name);
    }
    else if(tokens[current].value == std::string(1, '(')){
        current += 1;
        auto expr = parse_relational_expression();
        current += 1;
        return expr;
    }else if(tokens[current].value == "input"){
        current += 2;
        auto expr = parse_relational_expression();
        current += 1;
        return std::make_unique<input_node>(std::move(expr)); 
    }
    else {
        throw std::runtime_error("Unexpected token in parseFactor: " + tokens[current].value);
    }
}

void Var_Decal_Node::print(int indent) const {
    print_ident(indent);
    std::cout << "Variable: " << type << " " << name << std::endl;
}

void Var_Arr_Decal_Node::print(int indent) const {
    print_ident(indent);
    std::cout << "Variable array: " << type << " " << name << " size: " << std::endl;
    size->print(indent + 1);
}

void Func_Decal_Node::print(int indent) const {
    print_ident(indent);
    std::cout << "function: " << return_type << " " << name << " body: " << std::endl; //implement parameter printing
    for(int i = 0; i < body.size(); ++i){
        body[i]->print(indent + 1);
    }
}

void Var_Initialise_Node::print(int indent) const {
    print_ident(indent);
    std::cout << "Variable initialisation: " << name_of_var << " = " << value_of_var << std::endl;
}

void While_Node::print(int indent) const {
    print_ident(indent);
    std::cout << "While loop condition: " << std::endl;
    expression->print(indent+1);
    std::cout << std::endl;
    print_ident(indent);
    std::cout << "While loop body: " << std::endl;
    for(int i = 0; i < body.size(); ++i){
        body[i]->print(indent+1);
    }
}

void Else_Node::print(int indent) const {
    print_ident(indent);
    std::cout << "else body: " << std::endl;
    for(int i = 0; i < body.size(); ++i){
        body[i]->print(indent+1);
    }
}

void If_Node::print(int indent) const {
    print_ident(indent);
    std::cout << "If condition: " << std::endl;
    expression->print(indent+1);
    std::cout << std::endl;
    print_ident(indent);
    std::cout << "If body: " << std::endl;
    for(int i = 0; i < body.size(); ++i){
        body[i]->print(indent+1);
    }
}

void Number_Expr::print(int indent) const {
    print_ident(indent);
    std::cout << "Number: " << value << std::endl;
}

void Variable_Expr::print(int indent) const {
    print_ident(indent);
    std::cout << "Variable: " << name << std::endl;
}

void Binary_Expr::print(int indent) const {
    print_ident(indent);
    std::cout << "Binary Expression: " << std::endl;
    left->print(indent + 1);
    print_ident(indent + 1);
    std::cout << "operation: " << op << std::endl;
    right->print(indent + 1); 
}

void Assignment_Expr::print(int indent) const {
    print_ident(indent);
    std::cout << "Variable initialisation: " << name << " = " << std::endl;
    value->print(indent + 1);
}

void Rel_Operation::print(int indent) const {
    print_ident(indent);
    std::cout << "relational operation expression: " << std::endl;
    left_expression->print(indent + 1);
    print_ident(indent+1);
    std::cout << "operation: " << op << std::endl;
    right_expression->print(indent + 1);
}

void return_node::print(int indent) const {
    print_ident(indent);
    std::cout << "return node: " << std::endl;
    expression->print(indent + 1);
}

void output_node::print(int indent) const {
    print_ident(indent);
    std::cout << "output expression: " << std::endl;
    output_val->print(indent + 1);
}

void input_node::print(int indent) const {
    print_ident(indent);
    std::cout << "input expression: " << std::endl;
    input_val->print(indent + 1);
}

void parser::expect(std::string expected_token, const std::string& error_message, int current_position) {
    if(current_position >= tokens.size() || tokens[current_position].value != expected_token){
        throw std::runtime_error(error_message + " at line " + std::to_string(tokens[current_position].line) + ", column " + std::to_string(tokens[current_position].column));
    }
}

void Var_Decal_Node::accept(Visitor& visitor){
    visitor.visit(this);
}
void Var_Arr_Decal_Node::accept(Visitor& visitor){
    visitor.visit(this);
}
void Func_Decal_Node::accept(Visitor& visitor){
    visitor.visit(this);
}
void Var_Initialise_Node::accept(Visitor& visitor){
    visitor.visit(this);
}
void While_Node::accept(Visitor& visitor){
    visitor.visit(this);
}
void If_Node::accept(Visitor& visitor){
    visitor.visit(this);
}
void Else_Node::accept(Visitor& visitor){
    visitor.visit(this);
}
void Number_Expr::accept(Visitor& visitor){
    visitor.visit(this);
}
void Variable_Expr::accept(Visitor& visitor){
    visitor.visit(this);
}
void Binary_Expr::accept(Visitor& visitor){
    visitor.visit(this);
}
void Assignment_Expr::accept(Visitor& visitor){
    visitor.visit(this);
}
void Rel_Operation::accept(Visitor& visitor){
    visitor.visit(this);
}
void return_node::accept(Visitor& visitor){
    visitor.visit(this);
}
void output_node::accept(Visitor& visitor){
    visitor.visit(this);
}
void input_node::accept(Visitor& visitor){
    visitor.visit(this);
}
