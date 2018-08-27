#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <string_view>

#include "../linaro_utils/common.h"
#include "../linaro_utils/utils.h"
#include "token.h"

namespace linaro {

class Lexer {
 public:
  Lexer(const char* filename);
  Lexer(const std::string& source);
  ~Lexer() {}
  Location& getLocation() { return current_location; }
  Token nextToken();

 private:
  void initLexer(const char* filename);
  void lexicalError(const Location& loc, const char* format, ...);

  Token number();
  Token identifier();
  Token linaroString();

  // When the 'function' keyword is found, find the entire source for the
  // function and store it
  std::string_view getFunctionSource();

  void advance(unsigned int steps = 1);
  bool skipWhitespace();
  inline char peek(unsigned int distance = 1) {
    return m_source_code[current + distance];
  }

  inline bool isDigit(char d) { return d >= '0' && d <= '9'; }
  inline bool isAlpha(char d) {
    return (d >= 'a' && d <= 'z') || (d >= 'A' && d <= 'Z') || (d == '_');
  }

  Token getNextToken();
  inline size_t offsetFromCursor() const { return (start + current) - cursor; }
  inline void syncCursor() { cursor += offsetFromCursor(); }

  inline Token constructToken(Token::TokenType type) {
    switch (type) {
      case Token::STRING:
      case Token::SYMBOL:
      case Token::NUMBER:
      case Token::UNKNOWN: {
        std::string_view sv{cursor, offsetFromCursor()};
        syncCursor();
        return Token(type, current_location, sv);
      }
      default:
        return Token(type, current_location);
    }
  }

  std::string m_source_code;
  const char* start;
  const char* cursor;
  size_t current = 0;
  char current_char;
  Location current_location;
};

}  // namespace linaro

#endif  // LEXER_H