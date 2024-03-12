#include "utils.h"

#include <iostream>
#include <string>
#include <sstream>
#include <variant>

#include "lexer.h"


enum tokenid {
    t_identifier=1000,
    t_directive,
    t_int,
};

struct my_lexer : lexer::lex_base<tokenid, int, std::string> {

    void set_up() {
        skip( lexer::hspc );
        rule( lexer::identifier(lexer::alpha | '_', lexer::alnum | '_'), t_identifier );
        rule( lexer::identifier('.', lexer::alnum | '_'), t_directive );    
        rule( lexer::prefixed(lexer::literal("$"), lexer::many_of(lexer::hex_digits, 1)),
            t_int, [](const lexer::lexeme& l, token_type& t)->void {
                auto value_text = l.text().substr(1); // skip $
                t.value = (int)strtol(value_text.c_str(), nullptr, 16);
            });
        rule( lexer::prefixed(lexer::literal("%"), lexer::many_of(lexer::bin_digits, 1)),
            t_int, [](const lexer::lexeme& l, token_type& t)->void {
                auto value_text = l.text().substr(1); // skip %
                t.value = (int)strtol(value_text.c_str(), nullptr, 2);
            });
        rule( lexer::many_of(lexer::digits, 1),
            t_int, [](const lexer::lexeme& l, token_type& t)->void {
                t.value = (int)strtol(l.text().c_str(), nullptr, 10);
            });
    }
};

std::ostream& operator<<(std::ostream& o, const my_lexer::token_type& t) {
    o << "(" << (int)t.tokenid << ") " << (std::string)t.text;
    if(std::holds_alternative<int>(t.value)) {
        o << " " << std::get<int>(t.value);
    } else if(std::holds_alternative<std::string>(t.value)) {
        o << " " << std::get<std::string>(t.value);
    }
    return o;
}

int main(int argc, const char** argv) {
    my_lexer lex;
    lex.set_up();
    std::istringstream iss("foo %01abc .123 45 $far bar %11");
    lexer::lex_stream istr(iss);
    lex.push_stream(istr);
    while(auto token = lex.next_token()) {
        if(!token) break;
        std::cout << *token << std::endl;
    }
    return EXIT_SUCCESS;
}
