#ifndef TOKEN_H
#define TOKEN_H

#include <ostream>
#include <string>
#include <string_view>

#include "../linaro_utils/common.h"
#include "../vm/value.h"

namespace linaro {

#define TOKENS(T, K, C)                         \
  /* End of file. */                            \
  T(END, "EOF", 0)                              \
  T(EOS, "EOS", 0)                              \
                                                \
  /* Punctuators  */                            \
  T(LPAREN, "(", 16)                            \
  T(RPAREN, ")", 0)                             \
  T(LSB, "[", 16)                               \
  T(RSB, "]", 0)                                \
  T(LCB, "{", 0)                                \
  T(RCB, "}", 0)                                \
  T(COLON, ":", 0)                              \
  T(SEMICOLON, ";", 0)                          \
  T(PERIOD, ".", 16)                            \
  T(CONDITIONAL, "?", 3)                        \
  T(INCR, "++", 16)                             \
  T(DECR, "--", 16)                             \
  T(HASH, "#", 16)      /* Func call sync */    \
  T(AMPERSAND, "&", 16) /* Func call async  */  \
                                                \
  /* Assignment operators.. */                  \
  T(ASSIGN, "=", 2)                             \
                                                \
  /* Binary operators sorted by precedence. */  \
  T(OR, "or", 4)                                \
  T(AND, "and", 5)                              \
  T(COMMA, ",", 0)                              \
  T(ADD, "+", 12)                               \
  T(SUB, "-", 12)                               \
  T(MUL, "*", 13)                               \
  T(DIV, "/", 13)                               \
  T(MOD, "%", 13)                               \
  T(EXP, "^", 14)                               \
                                                \
  /* Compare operators sorted by precedence. */ \
  T(EQ, "==", 9)                                \
  T(NE, "!=", 9)                                \
  T(LT, "<", 10)                                \
  T(GT, ">", 10)                                \
  T(LTE, "<=", 10)                              \
  T(GTE, ">=", 10)                              \
                                                \
  /* Unary operators. */                        \
  T(NOT, "!", 15)                               \
                                                \
  /* Keywords */                                \
  K(BREAK, "break", 0)                          \
  K(FUNCTION, "fn", 0)                          \
  K(ELSE, "else", 0)                            \
  K(FOR, "for", 0)                              \
  K(IF, "if", 0)                                \
  K(NEW, "new", 15)                             \
  K(RETURN, "ret", 0)                           \
  K(THIS, "this", 0)                            \
  K(SUPER, "super", 0)                          \
  K(WHILE, "while", 0)                          \
  K(CLASS, "class", 0)                          \
  K(PRINT, "print", 0)                          \
  K(METHOD, "method", 0)                        \
  K(FIELD, "field", 0)                          \
  K(CONSTRUCTOR, "constructor", 0)              \
  K(INHERITS, "inherits", 0)                    \
                                                \
  K(UNKNOWN, "unknown", 0)                      \
                                                \
  /* Literals  */                               \
  K(NOLL, "null", 0)                            \
  K(TRUE, "true", 0)                            \
  K(FALSE, "false", 0)                          \
  T(NUMBER, nullptr, 0)                         \
  T(STRING, nullptr, 0)                         \
                                                \
  /* Identifiers (not keywords). */             \
  T(SYMBOL, nullptr, 0)

struct Location {
  friend std::ostream& operator<<(std::ostream& s, const Location& loc) {
    s << loc.file << ":" << loc.line << ":" << loc.col << ":";
    return s;
  }
  const char* file;
  int line;
  int col;
};

// Enum representation of TOKENS
#define T(type, name, precedence) type,
enum class TokenType : uint8_t { TOKENS(T, T, T) NUM_TOKENS };
#undef T

// Linaro token
class Token {
 public:
  Token() {}
  // The rest, no value representation, printed using "tokenString()"
  Token(TokenType type, const Location& location)
      : m_type{type}, m_location{location} {}
  // For string literals, symbols and numbers.
  Token(TokenType type, const Location& location, const std::string_view& str)
      : m_type{type}, m_location{location}, m_str{str} {}
  Token(TokenType type, const std::string_view& str)
      : m_type{type}, m_str{str} {}

  ~Token() {}

  TokenType type() const { return m_type; }
  const Location& getLocation() const { return m_location; }

  void setLocation(const Location& loc) { m_location = loc; }
  void setColumn(int col) { m_location.col = col; }
  void setLineNumber(int line) { m_location.line = line; }
  void setHadNewlineBefore(bool b) { had_newline_before = b; }
  bool hadNewlineBefore() const { return had_newline_before; }

  // E.g token LPAREN becomes the strin "LPAREN"
  static const char* tokenName(TokenType type) {
    CHECK(type < TokenType::NUM_TOKENS);
    return token_name[(uint8_t)type];
  }

  // E.g token LPAREN becomes "("
  static const char* tokenString(TokenType type) {
    CHECK(type < TokenType::NUM_TOKENS);
    return token_string[(uint8_t)type];
  }

  std::string_view asString() const;

  static int precedence(TokenType type) {
    CHECK(type < TokenType::NUM_TOKENS);
    return Precedence[(uint8_t)type];
  }

  static bool isLogicalOp(TokenType op) {
    return op == TokenType::OR || op == TokenType::AND;
  }
  static bool isBinaryOp(TokenType op) {
    return (TokenType::OR <= op && op <= TokenType::GTE) ||
           op == TokenType::PERIOD;
  }

  static bool isComparisonOp(TokenType op) {
    return TokenType::EQ <= op && op <= TokenType::GTE;
  }
  static bool isUnaryOp(TokenType op) {
    return op == TokenType::NOT || op == TokenType::ADD || op == TokenType::SUB;
  }

  static bool isCountOp(TokenType op) {
    return op == TokenType::INCR || op == TokenType::DECR;
  }
  static bool isAssignOp(TokenType type) { return type == TokenType::ASSIGN; }
  friend std::ostream& operator<<(std::ostream& cout, const Token& tok);

 private:
#define T(type, name, precedence) #type,
  constexpr static const char* const token_name[(uint8_t)TokenType::NUM_TOKENS]{
      TOKENS(T, T, T)};
#undef T

#define T(type, name, precedence) name,
  constexpr static const char* const
      token_string[(uint8_t)TokenType::NUM_TOKENS]{TOKENS(T, T, T)};
#undef T

#define T(name, string, precedence) precedence,
  constexpr static const int8_t Precedence[(uint8_t)TokenType::NUM_TOKENS]{
      TOKENS(T, T, T)};
#undef T

  TokenType m_type;
  Location m_location;
  // String view of strings, symbols and numbers. Points straight into source
  // code.
  std::string_view m_str;

  // True if token had atleast 1 '\n' before it.
  bool had_newline_before;
};
}  // Namespace linaro

#endif  // TOKEN_H