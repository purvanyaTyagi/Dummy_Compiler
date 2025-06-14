#include<iostream>
#include<string>
#include<vector>
#include<unordered_set>
#include<cctype>
#include "lexer.h"
//#include "parser.h"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Keywords: return "KEYWORD";
        case TokenType::identifiers: return "IDENTIFIER";
        case TokenType::numbers: return "NUMBER";
        case TokenType::operators: return "OPERATOR";
        case TokenType::delimiters: return "DELIMITER";
        default: return "UNKNOWN";
    }

}

std::vector<Token> Lexer::return_tokenized(){
    return tokenized_text;
}

void Lexer::tokenize(const std::string& source){
    size_t i = 0;
    size_t line = 1;
    size_t col_number = 1;
    while(i < source.length()){
        if(source[i] == '\n'){
            line += 1;
            i++;
            col_number = 1;
            continue;
        }
        if(std::isspace(source[i])){
            ++i;
            col_number++;
            continue;
        }
        
        if(std::isalpha(source[i]) || source[i] == '_'){
            std::string word;
            while(i < source.length() && (std::isalnum(source[i]) || source[i] == '_')){
                word += source[i];
                i++;
                col_number++;
            }
            if(keywords.find(word) != keywords.end()){
                Token token;
                token.value = word;
                token.type = TokenType::Keywords;
                token.line = line;
                token.column = col_number - word.length();
                tokenized_text.push_back(token);
            }else{
                Token token;
                token.value = word;
                token.type = TokenType::identifiers;
                token.line = line;
                token.column = col_number - word.length();
                tokenized_text.push_back(token);
            }
        }
        else if(std::isdigit(source[i])){
            std::string word;
            while(i < source.length() && (std::isdigit(source[i]))){
                word += source[i];
                i++;
                col_number++;
            }
            Token token;
            token.value = word;
            token.type = TokenType::numbers;
            token.line = line;
            token.column = col_number - word.length();
            tokenized_text.push_back(token);
        }
        else if(operators.find(std::string(1, source[i])) != operators.end()){
            if(operators.find(std::string(1, source[i+1])) != operators.end()){
                std::string word;
                word += source[i];
                word += source[i+1];
                Token token;
                token.value = word;
                token.type = TokenType::operators;
                token.line = line;
                token.column = col_number;
                tokenized_text.push_back(token);
                i += 2;
                col_number += 2;
                continue;
            } 
            Token token;
            token.value = std::string(1, source[i]);
            token.type = TokenType::operators;
            token.line = line;
            token.column = col_number;
            tokenized_text.push_back(token);
            i++;
            col_number++;
        }
        else if(delimiters.find(std::string(1, source[i])) != delimiters.end()){
            Token token;
            token.value = std::string(1, source[i]);
            token.type = TokenType::delimiters;
            token.line = line;
            token.column = col_number;
            tokenized_text.push_back(token);
            i++;
            col_number++;
        }
        else{
            i++;
            col_number++;
        }

    }
}

void Lexer::output(){
    for(std::vector<Token>::iterator it = tokenized_text.begin(); it != tokenized_text.end(); ++it){
        std::cout << "Token: " 
                    << tokenTypeToString(it->type) 
                    << "(" << it->value << ") "
                    << "at line " << it->line << ", col " << it->column 
                    << std::endl;
    }
}

