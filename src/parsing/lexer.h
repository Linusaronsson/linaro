#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <string_view>

#include "../linaro_utils/common.h"
#include "../linaro_utils/utils.h"
#include "token.h"

namespace Linaro {

class Lexer {
 public:
  Lexer(const char* filename);
  Lexer(const std::string& source);
  ~Lexer() {}
  Location& getLocation() { return m_current_location; }
  Token nextToken();
  void readSource(const char* file) { m_source_code = readFile(file); }

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
  inline char peek(unsigned int distance = 1);
  inline bool isDigit(char d);
  inline bool isAlpha(char d);
  Token getNextToken();
  inline size_t offsetFromCursor() const;
  inline void syncCursor();
  inline Token constructToken(TokenType type);

  std::string m_source_code;
  const char* m_start;
  const char* m_cursor;
  size_t m_current = 0;
  char m_current_char;
  Location m_current_location;
};  // namespace linaro

}  // namespace Linaro

#endif  // LEXER_H