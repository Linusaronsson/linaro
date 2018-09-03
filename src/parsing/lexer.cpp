#include "lexer.h"

#include <string>
#include <vector>

#include "../linaro_utils/common.h"

namespace Linaro {

Lexer::Lexer(const char* filename) : m_source_code{readFile(filename)} {
  initLexer(filename);
}

Lexer::Lexer(const std::string& source) : m_source_code{source} {
  initLexer("VM");
}

void Lexer::initLexer(const char* filename) {
  m_cursor = m_start = m_source_code.c_str();
  m_current_char = m_source_code[0];
  m_current_location = {filename, 0, 0};
}

void Lexer::lexicalError(const Location& loc, const char* format, ...) {
  va_list args;
  va_start(args, format);
  Error::reportErrorAt(loc, Error::LexicalError, format, args);
  va_end(args);
}

char Lexer::peek(unsigned int distance) {
  return m_source_code[m_current + distance];
}

bool Lexer::isDigit(char d) { return d >= '0' && d <= '9'; }
bool Lexer::isAlpha(char d) {
  return (d >= 'a' && d <= 'z') || (d >= 'A' && d <= 'Z') || (d == '_');
}

size_t Lexer::offsetFromCursor() const {
  return (m_start + m_current) - m_cursor;
}
void Lexer::syncCursor() { m_cursor += offsetFromCursor(); }
Token Lexer::constructToken(TokenType type) {
  switch (type) {
    case TokenType::STRING:
    case TokenType::SYMBOL:
    case TokenType::NUMBER:
    case TokenType::UNKNOWN: {
      std::string_view sv{m_cursor, offsetFromCursor()};
      syncCursor();
      return Token(type, m_current_location, sv);
    }
    default:
      return Token(type, m_current_location);
  }
}

// skips whitespace and also checks if a '\n' was amongst the skipped
// whitespace.
bool Lexer::skipWhitespace() {
  bool had_new_line = false;
  while (m_current_char == ' ' || m_current_char == '\t' ||
         m_current_char == '\r' || m_current_char == '\n') {
    if (m_current_char == '\n') {
      had_new_line = true;
      m_current_location.line++;
      m_current_location.col = 0;
    }
    advance();
  }
  return had_new_line;
}

Token Lexer::nextToken() {
  Location save_loc = m_current_location;
  bool b = skipWhitespace();
  syncCursor();
  Token temp = getNextToken();
  temp.setHadNewlineBefore(b);
  temp.setLocation(save_loc);
  return temp;
}

Token Lexer::getNextToken() {
  m_current_location.col++;
  while (m_current_char != '\0') {
    if (isDigit(m_current_char)) {
      return number();
    }

    if (isAlpha(m_current_char)) {
      return identifier();  // reserved keyword or symbol
    }

    if (m_current_char == '"') {
      advance();
      return linaroString();
    }

    switch (m_current_char) {
      // Guaranteed single char tokens
      case '*':
        advance();
        return constructToken(TokenType::MUL);
      case '%':
        advance();
        return constructToken(TokenType::MOD);
      case '/':
        advance();
        return constructToken(TokenType::DIV);
      case '^':
        advance();
        return constructToken(TokenType::EXP);
      case ')':
        advance();
        return constructToken(TokenType::RPAREN);
      case '(':
        advance();
        return constructToken(TokenType::LPAREN);
      case ']':
        advance();
        return constructToken(TokenType::RSB);
      case '[':
        advance();
        return constructToken(TokenType::LSB);
      case '}':
        advance();
        return constructToken(TokenType::RCB);
      case '{':
        advance();
        return constructToken(TokenType::LCB);
      case ',':
        advance();
        return constructToken(TokenType::COMMA);
      case '.':
        advance();
        return constructToken(TokenType::PERIOD);
      case ':':
        advance();
        return constructToken(TokenType::COLON);
      case '#':
        advance();
        return constructToken(TokenType::HASH);
      case '&':
        advance();
        return constructToken(TokenType::AMPERSAND);
      // EOS (semicolon).
      case ';':
        while (m_current_char == ';') advance();
        return constructToken(TokenType::SEMICOLON);

      // Potential double char tokens
      case '=':
        if (peek() == '=') {
          advance(2);
          return constructToken(TokenType::EQ);
        }
        advance();
        return constructToken(TokenType::ASSIGN);
      case '+':
        if (peek() == '+') {
          advance(2);
          return constructToken(TokenType::INCR);
        }
        advance();
        return constructToken(TokenType::ADD);
      case '-':
        if (peek() == '-') {
          advance(2);
          return constructToken(TokenType::DECR);
        }
        advance();
        return constructToken(TokenType::SUB);
      case '!':
        if (peek() == '=') {
          advance(2);
          return constructToken(TokenType::NE);
        }
        advance();
        return constructToken(TokenType::NOT);

      case '>':
        if (peek() == '=') {
          advance(2);
          return constructToken(TokenType::GTE);
        }
        advance();
        return constructToken(TokenType::GT);

      case '<':
        if (peek() == '=') {
          advance(2);
          return constructToken(TokenType::LTE);
        }
        advance();
        return constructToken(TokenType::LT);
    }

    // If it gets here, the character is unknown.
    //  lexicalError(current_location, "Unrecognized token: %c\n",
    //  current_char);
    advance();
    return constructToken(TokenType::UNKNOWN);
  }

  // Reached end of source file, return EOF token.
  return constructToken(TokenType::END);
}

void Lexer::advance(unsigned int steps) {
  if (m_current_char == '\0') return;
  m_current += steps;
  m_current_char = m_source_code[m_current];
}

Token Lexer::number() {
  while (isDigit(m_current_char)) {
    advance();
  }

  // decimal point (This means it's a float, but all numbers are stored as
  // doubles for now. Wil be changed when optimizing the performance of the
  // interpreter.)
  if (m_current_char == '.' && isDigit(peek())) {
    advance();
    while (isDigit(m_current_char)) {
      advance();
    }
  }
  return constructToken(TokenType::NUMBER);
  // Add support for numbers written in scientific notation
}

Token Lexer::identifier() {
  while (isAlpha(m_current_char) || isDigit(m_current_char)) {
    advance();
  }

  std::string_view result{m_cursor, offsetFromCursor()};

  // If it's a reserved keyword:
  if (result == "if") return constructToken(TokenType::IF);
  if (result == "fn") return constructToken(TokenType::FUNCTION);
  if (result == "else") return constructToken(TokenType::ELSE);
  if (result == "for") return constructToken(TokenType::FOR);
  if (result == "while") return constructToken(TokenType::WHILE);
  if (result == "print") return constructToken(TokenType::PRINT);
  if (result == "ret") return constructToken(TokenType::RETURN);
  if (result == "and") return constructToken(TokenType::AND);
  if (result == "or") return constructToken(TokenType::OR);
  if (result == "class") return constructToken(TokenType::CLASS);
  if (result == "method") return constructToken(TokenType::METHOD);
  if (result == "field") return constructToken(TokenType::FIELD);
  if (result == "constructor") return constructToken(TokenType::CONSTRUCTOR);
  if (result == "inherits") return constructToken(TokenType::INHERITS);
  if (result == "new") return constructToken(TokenType::NEW);
  if (result == "this") return constructToken(TokenType::THIS);
  if (result == "super") return constructToken(TokenType::SUPER);

  // literals that are keywords
  if (result == "true") return constructToken(TokenType::TRUE);
  if (result == "false") return constructToken(TokenType::FALSE);
  if (result == "null") return constructToken(TokenType::NOLL);

  // Otherwise it's a symbol of some sort. (E.g. a variable or function name).
  return constructToken(TokenType::SYMBOL);
}

Token Lexer::linaroString() {
  std::string str;
  for (;;) {
    if (m_current_char == '"') {
      advance();
      break;
    }
    if (m_current_char == '\0') {
      lexicalError(m_current_location, "Unterminated string");
      break;
    }
    if (m_current_char == '\\') {
      switch (peek()) {
        case 'n':
          str.push_back('\n');
          break;
        case 't':
          str.push_back('\t');
          break;
      }
      advance();
    } else {
      str.push_back(m_current_char);
    }
    advance();
  }

  return Token(TokenType::STRING, std::string_view(str));
}

}  // namespace Linaro