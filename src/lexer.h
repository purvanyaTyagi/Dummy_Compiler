#pragma once
#include<iostream>
#include<vector>
#include<cctype>
#include<unordered_set>

const std::unordered_set<std::string> keywords = {"int", "void", "if", "else", "while", "return", "input", "output"};
const std::unordered_set<std::string> operators = {"+", "-", "*", "/", "=", "==", "!=", "<", "<=", ">", ">="};
const std::unordered_set<std::string> delimiters = { "{", "}", "(", ")", "[", "]", ";", " ", ","};

enum class TokenType{
    Keywords, identifiers, numbers, operators, delimiters
};
struct Token{
    std::string value;
    TokenType type;
    int line; 
    int column;
};

std::string tokenTypeToString(TokenType type);

class Lexer{
    private:
        std::vector<Token> tokenized_text;
    public:
        void tokenize(const std::string& source);
        std::vector<Token> return_tokenized();
        void output();
        void next_token();
        void peek_char();
        void skip_whitespace();
        void read_number();
        void read_identifier();
        void handle_comments();
};

