#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../ast/expression.h"
#include "../ast/statement.h"
#include "../linaro_utils/common.h"
#include "lexer.h"
#include "token.h"

namespace Linaro {

class Parser {
 public:
  Parser(const char* filename);
  ~Parser() {}
  FunctionLiteralPtr parse();

 private:
  void syntaxError(const Location& loc, const char* format, ...);
  inline TokenType peek(int steps = 1);
  inline Token lookahead(int steps = 1);
  inline void rotateBuffer();
  inline TokenType currentToken() const { return current_token.type(); }
  inline TokenType previousToken() const { return previous_token.type(); }
  void nextToken();

  void unexpectedToken(const Token& tok);
  bool match(TokenType expected);
  void expect(TokenType expected, const char* error_message);
  void consume(TokenType type, const char* error_message);
  void expectEndOfStatement(const char* error_message);
  void synchronize();
  void skipBlock();  // Skip a block if the initialization of it failed.

  bool isValidReferenceIdentifier(ExpressionPtr id, bool is_assignment = false);

  // Constructs a Value from a literal.
  Value constructValue(const Token& tok);

  const Location& loc() { return m_lex.getLocation(); }

  /* --- Expression evaluation --- */

  ExpressionPtr parseExpression(int precedence);
  ExpressionPtr parseUnaryPrefixOperation();
  ExpressionPtr parseBinaryOperation(ExpressionPtr& left);
  ExpressionPtr parseUnaryPostfixOperation(ExpressionPtr& left,
                                           const Token& tok);
  ExpressionPtr parseCall(ExpressionPtr& left);
  FunctionLiteralPtr parseFunctionLiteral(std::string_view name,
                                          FunctionType type);
  ExpressionPtr parseArrayLiteral();

  /* --- Statements --- */

  // Dispatches to one of the possible statements below,
  // depending on the current token.
  StatementPtr parseStatement();

  // Helper method for parsing and adding a statement to a block
  void addStatement(BlockPtr& block);

  BlockPtr parseBlock();
  StatementPtr parseIfStatement();
  StatementPtr parseWhileStatement();

  // return/print
  template <class T>
  StatementPtr parseSingleExpressionStatement();

  // Declarations (that are visited before other statements in AST)
  StatementPtr parseFunctionDeclaration();
  //  void parseClassDeclaration(StatementPtr& stmt);

  static const int buffer_size = 6;
  Lexer m_lex;
  Token current_token;
  Token previous_token;
  Token buffer[buffer_size];  // circular lookahead buffer
  int buffer_index = 0;
  bool hadError = false;
};

}  // namespace Linaro

#endif  // PARSER_H