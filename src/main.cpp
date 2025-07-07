#include<iostream>
#include "lexer.h"
#include "parser.h"
#include "semantics.h"

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
        symbol_table symbol_table_1;
        parser_1.parse_program();
        parser_1.print_tree();
        // symbol_table_1.contruct_symbol_table(parser_1.return_parsed_code());
        // std::vector<Scope> symbol_table_out = symbol_table_1.return_symbol_table();


        // parser parser_2(lexer.return_tokenized());
        Semantic_Analyser analyse_symantics;
        // // parser_2.parse_program();
        analyse_symantics.Analyse_code(parser_1.return_parsed_code());
        return 0;
    }else{
        std::cout << "no code received"; 
        return 0;
    }
}
