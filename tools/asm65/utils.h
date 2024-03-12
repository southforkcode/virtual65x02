#ifndef __UTILS_H
#define __UTILS_H

#include <sstream>
#include <string>

namespace utils {

struct string_builder {
    string_builder() { };
    template <typename T>
    string_builder& operator <<(const T& v) { _oss << v; return *this; }
    operator std::string() const { return _oss.str(); }
    std::ostringstream _oss;
};

}

// start string macro
#define SS() utils::string_builder()

#endif
