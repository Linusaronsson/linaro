#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace linaro {

struct Location {
    int m_line;
    int m_col;
    std::string m_filename;
};

class Token {

};

} // Namespace linaro

#endif // TOKEN_H