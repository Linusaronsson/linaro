#include "lexer.h"

#include <string>
#include <vector>

#include "../linaro_utils/common.h"

namespace linaro {

Lexer::Lexer(const char* filename) {
  m_source_code = readFile(filename);
  initLexer(filename);
}

Lexer::Lexer(const std::string& source) {
  m_source_code = source;
  initLexer("VM");
}

void Lexer::initLexer(const char* filename) {
  cursor = start = m_source_code.c_str();
  current_char = m_source_code[0];
  current_location = {filename, 0, 0};
}

void Lexer::lexicalError(const Location& loc, const char* format, ...) {
  va_list args;
  va_start(args, format);
  Error::reportErrorAt(loc, Error::LexicalError, format, args);
  va_end(args);
}

// skips whitespace and also checks if a '\n' was amongst the skipped
// whitespace.
bool Lexer::skipWhitespace() {
  bool had_new_line = false;
  while (current_char == ' ' || current_char == '\t' || current_char == '\r' ||
         current_char == '\n') {
    if (current_char == '\n') {
      had_new_line = true;
      current_location.line++;
      current_location.col = 0;
    }
    advance();
  }
  return had_new_line;
}

Token Lexer::nextToken() {
  Location save_loc = current_location;
  bool b = skipWhitespace();
  syncCursor();
  Token temp = getNextToken();
  temp.setHadNewlineBefore(b);
  temp.setLocation(save_loc);
  return temp;
}

Token Lexer::getNextToken() {
  current_location.col++;
  while (current_char != '\0') {
    if (isDigit(current_char)) {
      return number();
    }

    if (isAlpha(current_char)) {
      return identifier();  // reserved keyword or symbol
    }

    if (current_char == '"') {
      advance();
      return linaroString();
    }

    switch (current_char) {
      // Guaranteed single char tokens
      case '*':
        advance();
        return constructToken(Token::MUL);
      case '%':
        advance();
        return constructToken(Token::MOD);
      case '/':
        advance();
        return constructToken(Token::DIV);
      case '^':
        advance();
        return constructToken(Token::EXP);
      case ')':
        advance();
        return constructToken(Token::RPAREN);
      case '(':
        advance();
        return constructToken(Token::LPAREN);
      case ']':
        advance();
        return constructToken(Token::RSB);
      case '[':
        advance();
        return constructToken(Token::LSB);
      case '}':
        advance();
        return constructToken(Token::RCB);
      case '{':
        advance();
        return constructToken(Token::LCB);
      case ',':
        advance();
        return constructToken(Token::COMMA);
      case '.':
        advance();
        return constructToken(Token::PERIOD);
      case ':':
        advance();
        return constructToken(Token::COLON);

      // EOS (semicolon).
      case ';':
        while (current_char == ';') advance();
        return constructToken(Token::SEMICOLON);

      // Potential double char tokens
      case '=':
        if (peek() == '=') {
          advance(2);
          return constructToken(Token::EQ);
        }
        advance();
        return constructToken(Token::ASSIGN);
      case '+':
        if (peek() == '+') {
          advance(2);
          return constructToken(Token::INCR);
        }
        advance();
        return constructToken(Token::ADD);
      case '-':
        if (peek() == '-') {
          advance(2);
          return constructToken(Token::DECR);
        }
        advance();
        return constructToken(Token::SUB);
      case '!':
        if (peek() == '=') {
          advance(2);
          return constructToken(Token::NE);
        }
        advance();
        return constructToken(Token::NOT);

      case '>':
        if (peek() == '=') {
          advance(2);
          return constructToken(Token::GTE);
        }
        advance();
        return constructToken(Token::GT);

      case '<':
        if (peek() == '=') {
          advance(2);
          return constructToken(Token::LTE);
        }
        advance();
        return constructToken(Token::LT);
    }

    // If it gets here, the character is unknown.
    //  lexicalError(current_location, "Unrecognized token: %c\n",
    //  current_char);
    advance();
    return constructToken(Token::UNKNOWN);
  }

  // Reached end of source file, return EOF token.
  return constructToken(Token::END);
}

void Lexer::advance(unsigned int steps) {
  if (current_char == '\0') return;
  current += steps;
  current_char = m_source_code[current];
}

Token Lexer::number() {
  while (isDigit(current_char)) {
    advance();
  }

  // decimal point (This means it's a float, but all numbers are stored as
  // doubles for now. Wil be changed when optimizing the performance of the
  // interpreter.)
  if (current_char == '.' && isDigit(peek())) {
    advance();
    while (isDigit(current_char)) {
      advance();
    }
  }
  return constructToken(Token::NUMBER);
  // Add support for numbers written in scientific notation
}

Token Lexer::identifier() {
  while (isAlpha(current_char) || isDigit(current_char)) {
    advance();
  }

  std::string_view result{cursor, offsetFromCursor()};

  // If it's a reserved keyword:
  if (result == "if") return constructToken(Token::IF);
  if (result == "else") return constructToken(Token::ELSE);
  if (result == "for") return constructToken(Token::FOR);
  if (result == "while") return constructToken(Token::WHILE);
  if (result == "print") return constructToken(Token::PRINT);
  if (result == "function") return constructToken(Token::FUNCTION);
  if (result == "return") return constructToken(Token::RETURN);
  if (result == "and") return constructToken(Token::AND);
  if (result == "or") return constructToken(Token::OR);
  if (result == "class") return constructToken(Token::CLASS);
  if (result == "method") return constructToken(Token::METHOD);
  if (result == "field") return constructToken(Token::FIELD);
  if (result == "constructor") return constructToken(Token::CONSTRUCTOR);
  if (result == "inherits") return constructToken(Token::INHERITS);
  if (result == "new") return constructToken(Token::NEW);
  if (result == "this") return constructToken(Token::THIS);
  if (result == "super") return constructToken(Token::SUPER);

  // literals that are keywords
  if (result == "true") return constructToken(Token::TRUE);
  if (result == "false") return constructToken(Token::FALSE);
  if (result == "null") return constructToken(Token::NOLL);

  // Otherwise it's a symbol of some sort. (E.g. a variable or function name).
  return constructToken(Token::SYMBOL);
}

Token Lexer::linaroString() {
  for (;;) {
    if (current_char == '"') {
      advance();
      break;
    }
    if (current_char == '\0') {
      lexicalError(current_location, "Unterminated string");
      break;
    }
    advance();
  }

  return constructToken(Token::STRING);
}

}  // namespace linaro