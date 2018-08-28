#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../ast/expression.h"
#include "../ast/statement.h"
#include "../linaro_utils/common.h"
#include "lexer.h"
#include "token.h"

namespace linaro {

class Parser {
 public:
  Parser(const Lexer& lex);
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
  void consume(TokenType type, const char* error_message);
  void expectEndOfStatement(const char* error_message);
  void synchronize();
  void skipBlock();  // Skip a block if the initialization of it failed.

  bool isValidReferenceIdentifier(ExpressionPtr id, bool is_assignment = false);

  // Constructs a Value from a literal.
  Value constructValue(const Token& tok);

  const Location& loc() { return m_lex.getLocation(); }

  // Dispatches to one of the possible statements below,
  // depending on the current token.
  StatementPtr parseStatement();

  void parseBlock(StatementPtr& stmt);
  void parseIfStatement(StatementPtr& stmt);
  void parseWhileStatement(StatementPtr& stmt);
  void parseVariableDeclaration(StatementPtr& stmt);

  void transformBinOpWithNumberLiterals(Expression** x, Expression* y,
                                        const Token& op, const Location& loc);

  // return/print
  template <class T>
  void parseSingleExpressionStatement(StatementPtr& stmt);

  // Declarations (that are visited before other statements in AST)
  void parseFunctionDeclaration(StatementPtr& stmt);
  void parseClassDeclaration(StatementPtr& stmt);

  // Expression evaluation
  void parseExpression(ExpressionPtr& arg, int precedence);
  void parseUnaryPrefixOperation(ExpressionPtr& left);
  void parseBinaryOperation(ExpressionPtr& left);  // handles postfix too
  void parseCall(ExpressionPtr& left, CallType type);
  // ExpressionPtr parseMemberExpression(ExpressionPtr left);
  // ExpressionPtr parseAssignment(ExpressionPtr left, const Token& tok);
  void parseFunctionLiteral(FunctionLiteralPtr& fn, std::string name,
                            FunctionType type);

  static const int buffer_size = 6;
  Lexer m_lex;
  Token current_token;
  Token previous_token;
  Token buffer[buffer_size];  // circular lookahead buffer
  int buffer_index = 0;
  bool hadError = false;
};

}  // namespace linaro

#endif  // PARSER_H