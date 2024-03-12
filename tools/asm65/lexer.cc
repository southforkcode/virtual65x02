#include "lexer.h"

namespace lexer {

std::ostream& operator<<(std::ostream& ostr, const char_class& chcl) {
    for(int i=0; i<128; i++) {
        if(chcl.contains(i)) {
            if (isprint(i)) ostr << (char)i;
            else ostr << "(" << i << ")";
        } 

    }
    return ostr;
}
const char_class hspc(" \t");
const char_class vspc("\r\n");
const char_class ws = hspc | vspc;
const char_class lower('a','z');
const char_class upper('A','Z');
const char_class digits('0','9');
const char_class alpha = lower | upper;
const char_class alnum = alpha | digits;
const char_class hex_digits = digits | char_class('a','f') | char_class('A','F');
const char_class bin_digits('0','1');

std::ostream& operator<<(std::ostream& o, const lex_stream_state& s) {
    o << "[" << (int)s.pos << "]";
    return o;
}

opt_lexeme identifier::try_match(lex_stream& istr) const {
    auto state = istr.state();
    if(_start.contains(istr.peek())) {
        std::string s = istr.read_while(_start) + istr.read_while(_rest);
        return istr.accept(state);
    }
    return istr.reject(state);
}

opt_lexeme literal::try_match(lex_stream& lstr) const {
    auto state = lstr.state();
    if(_literal.length() < 1) return lstr.reject(state);
    for(int i=0; i<_literal.length(); ++i) {    
        if(lstr.peek() != _literal[i]) return lstr.reject(state);
        lstr.advance();
    }
    return lstr.accept(state);
}

opt_lexeme many_of::try_match(lex_stream& lstr) const {
    auto state = lstr.state();
    int count = 0;
    while(_chcl.contains(lstr.peek())) {
        std::cout << (int)lstr.get() << std::endl;
        ++count;
        //lstr.advance();
    }
    std::cout << _min << " " << count << std::endl;
    if(_min > 0 && _min > count) return lstr.reject(state);
    return lstr.accept(state);
}

}