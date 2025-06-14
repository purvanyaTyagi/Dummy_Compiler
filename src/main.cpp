#include<iostream>
#include "lexer.h"
#include "parser.h"

int main(int argc, char* argv[]){
    // std::string code = R"(hello main() {
    //     int x;
    //     int y;
    //     x = 10;
    //     y = 5;
    //     z = (x + y) * (5 + x);
    //     int f;
    //     f = input(y);
    //     if ((x+5)*(y + (x + 2*input(x))) >= y) {
    //         output(x + y);
    //     } else {
    //         output(y + (x * 3));
    //     }
    //     while(x == y){
    //         output(x * y);
    //     }
    //     return 0;
    // })";
    if (argc > 1) {
        std::string code = argv[1];
        Lexer lexer;
        lexer.tokenize(code);
        lexer.output();
        parser parser_1(lexer.return_tokenized());
        parser_1.parse_program();
        parser_1.print_tree();
        return 0;
    }else{
        std::cout << "no code received"; 
        return 0;
    }
}
