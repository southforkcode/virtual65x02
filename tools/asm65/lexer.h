#ifndef __LEXER_H
#define __LEXER_H

#include <optional>
#include <vector>
#include <set>
#include <deque>
#include <string>
#include <istream>
#include <sstream>
#include <iostream>
#include <functional>
#include <exception>

#include "utils.h"

namespace lexer {

class lex_error : public std::runtime_error {
public:
    lex_error() : std::runtime_error("") { }
    lex_error(const std::string& msg) : std::runtime_error(""), _msg(msg) { }
    const char* what() const noexcept { return _msg.c_str(); }
private:
    std::string _msg;
};

struct char_class {
    constexpr char_class() : _chars{} { }
    constexpr char_class(int c) : _chars{} { insert(c); }
    constexpr char_class(int l, int h) : _chars{} { insert(l,h); }
    constexpr char_class(const char_class& one) : _chars {} {
        insert(one);
    }
    constexpr char_class(const char* cs) : _chars{} {
        insert(cs);
    }
    bool contains(int c) const { return _chars[c]; }
    constexpr void insert(int c) { _chars[c] = true; }
    constexpr void insert(int l, int h) { for(auto i=l; i<=h; ++i) insert(i); }
    constexpr void insert(const char_class& chcl) {
        for(int i=0; i<128; ++i) _chars[i] |= chcl._chars[i];
    }
    constexpr void insert(const char* cs) {
        while(cs && *cs) _chars[*cs++] = true;
    }
    bool _chars[128];
};

constexpr auto operator|(const char_class& c1, const char_class& c2) ->
    char_class
{
    char_class copy(c1);
    copy.insert(c2);
    return copy;
}

std::ostream& operator<<(std::ostream& ostr, const char_class& chcl);

extern const lexer::char_class hspc;
extern const lexer::char_class vspc;
extern const lexer::char_class ws;
extern const lexer::char_class lower;
extern const lexer::char_class upper;
extern const lexer::char_class digits;
extern const lexer::char_class alpha;
extern const lexer::char_class alnum;
extern const lexer::char_class hex_digits;
extern const lexer::char_class bin_digits;

struct lexeme {
    lexeme(char c) { _t = c; }
    lexeme(const std::string& s) { _t = s; }
    const std::string& text() const { return _t; }
    operator std::string() const { return text(); }
    std::string _t;
};

typedef std::optional<lexeme> opt_lexeme;

struct lex_stream_state {
    std::istream::pos_type pos;    
};

std::ostream& operator<<(std::ostream& o, const lex_stream_state& s);

struct lex_stream {
    typedef std::shared_ptr<lex_stream> sptr;
    lex_stream(std::istream& istr) : _istr(istr) { }
    void unget() { _istr.unget(); }
    auto peek() { return _istr.peek(); }
    auto get() { return _istr.get(); }
    void advance(int count=1) { while(count-->0) get(); }
    lex_stream_state state() const { return {.pos=_istr.tellg()}; }
    void restore(const lex_stream_state& state) { 
        _istr.seekg(state.pos);
    }
    bool eof() const { return _istr.eof(); }
    bool good() const { return _istr.good(); }

    opt_lexeme accept() {
        std::ostringstream _tok;
        auto stop = _istr.tellg();
        _istr.seekg(_pos);
        while(_istr.tellg() != stop) {
            _tok << (char)_istr.get();
        }
        return lexeme(_tok.str());
    }
    opt_lexeme accept(const lex_stream_state& st) {
        std::ostringstream ss;
        auto stop = _istr.tellg();        
        restore(st);
        while(_istr.tellg() != stop) {
            ss << (char)_istr.get();
        }
        return lexeme(ss.str()); 
    }
    opt_lexeme accept(const std::string& s) {
        return lexeme(s);
    }
    opt_lexeme reject(const lex_stream_state& st) { 
        restore(st); return {}; 
    }

    std::string read_while(const char_class& chcl, ssize_t max=-1) {
        std::ostringstream oss;
        while(chcl.contains(_istr.peek()) && max != 0) {
            oss << (char)_istr.get();
            if(max>0) --max;
        }
        return oss.str();
    }
    std::istream& _istr;
    std::istream::pos_type _pos;
};

struct lex_rule {
    virtual opt_lexeme try_match(lex_stream& istr) const = 0;
};

struct identifier : lex_rule {
    constexpr identifier(const char_class& start, const char_class& rest) :
        lex_rule(),
        _start(start), _rest(rest)
    { }
    opt_lexeme try_match(lex_stream& istr) const;
    const char_class _start;
    const char_class _rest;
};

struct literal : lex_rule {
    constexpr literal(const char* value) : lex_rule(),  _literal(value) { }
    opt_lexeme try_match(lex_stream& lstr) const;
    const std::string _literal;
};

struct many_of : lex_rule {
    constexpr many_of(const char_class& chcl, int min=0) : lex_rule(), _chcl(chcl), _min(min) { }
    opt_lexeme try_match(lex_stream& lstr) const;
    const int _min;
    const char_class _chcl;
};

template <typename P, typename T>
struct prefixed : lex_rule {
    constexpr prefixed(const P& prefix, const T& rule) :
        lex_rule(), _prefix(prefix), _rule(rule) { }
    opt_lexeme try_match(lex_stream& lstr) const {
        auto save = lstr.state();
        auto prefix_tok = _prefix.try_match(lstr);
        if(!prefix_tok) return lstr.reject(save);
        auto rule_tok = _rule.try_match(lstr);
        if(!rule_tok) return lstr.reject(save);
        return lstr.accept(save);
    }
    const P _prefix;
    const T _rule;
};

template <typename V=void>
struct lex_token {
    typedef std::shared_ptr<lex_token<V>> sptr;
    lex_token(const lexeme& l) : _lexeme(l), _value() {}
    lex_token(const lexeme& l, const V& v) : _lexeme(l), _value(v) { }
    const lexeme _lexeme;
    const V _value;
};

template <typename I, typename T>
struct lex_rule_instance_base {    
    typedef std::shared_ptr<lex_rule_instance_base<I,T>> sptr;
    typedef std::function<void(const lexeme&, T&)> eval_fn_type;

    lex_rule_instance_base(const I& i) : _token_id(i), _fn(nullptr) { }
    lex_rule_instance_base(const I& i, eval_fn_type fn) :
        _token_id(i), _fn(fn) { }
    virtual opt_lexeme try_match(lex_stream& ls) = 0;
    
    bool has_token_id() const {
        if(std::is_same<I, void>::value) {
            return false;
        }
        return true;
    }

    virtual void eval(const lexeme& l, T& t) { if(_fn) _fn(l,t); }

    const I _token_id;
    eval_fn_type _fn;
};

template <typename R, typename I, typename T>
struct lex_rule_instance : lex_rule_instance_base<I,T> {
    lex_rule_instance(const R& rule) : lex_rule_instance_base<I,T>(), _rule(rule) { }
    lex_rule_instance(const R& rule, const I& token_id)
        : lex_rule_instance_base<I,T>(token_id), _rule(rule) { }
    lex_rule_instance(
        const R& rule,
        const I& token_id,
        lex_rule_instance_base<I,T>::eval_fn_type fn) :
            lex_rule_instance_base<I,T>(token_id, fn), _rule(rule) { }

    opt_lexeme try_match(lex_stream& ls) {
        return _rule.try_match(ls);
    }
    R _rule;
};

template <typename V, typename... Args>
struct lex_base {

    typedef struct {
        V tokenid;
        int line;
        int col;
        std::string text;
        std::variant<Args...> value;
    } token_type;


    char_class _skip;
    std::deque<lex_stream::sptr> _streams;
    
    typedef lex_rule_instance_base<V, token_type> base_rule_type;

    std::vector< std::shared_ptr<base_rule_type>> _rules;

    lex_base() : _skip{} { }
    lex_base(const char_class& skip) : _skip(skip) { }

    void push_stream(lex_stream& s) { _streams.push_front(std::make_shared<lex_stream>(s)); }
    void pop_stream() { _streams.pop_front(); }
    lex_stream::sptr cur() { return _streams.at(0); }
    void skip_next() { cur()->read_while(_skip); }

    virtual void set_up() {}

    void skip(const char_class& cl) {
        _skip = cl;
    }
    
    template <typename R>
    std::shared_ptr<lex_rule_instance<R, V, token_type>>
    rule(const R& rule, const V& token) {
        auto rule_inst = std::make_shared<lex_rule_instance<R, V, token_type>>(rule, token);
        _rules.push_back(rule_inst);
        return rule_inst;
    }

    template <typename R>
    std::shared_ptr<lex_rule_instance<R, V, token_type>> rule(
        const R& rule, 
        const V& token,
        lex_rule_instance_base<V, token_type>::eval_fn_type fn
    ) {
        auto rule_inst = std::make_shared<lex_rule_instance<R, V, token_type>>(
            rule,
            token,
            fn);
        _rules.push_back(rule_inst);
        return rule_inst;
    }

    std::shared_ptr<token_type> next_token() {
        auto _lstr_sptr = cur();
        if(!_lstr_sptr) return nullptr;
        if(!_lstr_sptr->good()) return nullptr;
        skip_next();
        if(!_lstr_sptr->good()) return nullptr;
        for(auto rptr : _rules) {
            if(auto l = rptr->try_match(*_lstr_sptr)) {
                auto tt = std::make_shared<token_type>();
                if(rptr->has_token_id())
                    tt->tokenid = rptr->_token_id;
                std::cout << (int)tt->tokenid << std::endl;
                tt->text = l.value().text();
                std::cout << tt->text.length() << std::endl;
                rptr->eval(*l, *tt);
                return tt;
            }
        }
        throw lexer::lex_error(SS() << "lexical error at " << (char)_lstr_sptr->peek());
    }


};


}

#endif