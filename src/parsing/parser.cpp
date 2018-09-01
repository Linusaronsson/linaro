#include "parser.h"

#include <math.h>
#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "lexer.h"
#include "token.h"

namespace linaro {

Parser::Parser(const char* filename) : m_lex{Lexer(filename)} {
  // Fill buffer with the initial 6 tokens
  for (int i = 0; i < buffer_size; i++) buffer[i] = m_lex.nextToken();
  current_token = buffer[0];
  nextToken();
}

FunctionLiteralPtr Parser::parse() {
  Identifier argc(Token(TokenType::SYMBOL, std::string_view("argc")));
  Identifier argv(Token(TokenType::SYMBOL, std::string_view("argv")));
  std::vector<Identifier> main_args = {argc, argv};
  BlockPtr main_block = std::make_unique<Block>();
  while (currentToken() != TokenType::END) {
    addStatement(main_block);
  }
  return std::make_unique<FunctionLiteral>(
      FunctionType::top_level, "@main_function", main_args, main_block);
}

void Parser::syntaxError(const Location& loc, const char* format, ...) {
  va_list args;
  va_start(args, format);
  Error::reportErrorAt(loc, Error::SyntaxError, format, args);
  va_end(args);
  hadError = true;
}

Token Parser::lookahead(int steps) {
  return buffer[(buffer_index + steps - 1) % buffer_size];
}

TokenType Parser::peek(int steps) { return lookahead(steps).type(); }

void Parser::rotateBuffer() {
  buffer[buffer_index] = m_lex.nextToken();
  buffer_index = (buffer_index + 1) % buffer_size;
}

void Parser::nextToken() {
  previous_token = current_token;
  current_token = buffer[buffer_index];
  rotateBuffer();
}

void Parser::synchronize() {
  nextToken();
  while (currentToken() != TokenType::END) {
    switch (currentToken()) {
      case TokenType::CLASS:
      case TokenType::IF:
      case TokenType::PRINT:
      case TokenType::RETURN:
      case TokenType::SYMBOL:
      case TokenType::METHOD:
      case TokenType::CONSTRUCTOR:
      case TokenType::FIELD:
        return;
      default:
        UNREACHABLE();
    }
    nextToken();
  }
}

Value Parser::constructValue(const Token& tok) {
  switch (tok.type()) {
    case TokenType::TRUE:
      return Value(true);
    case TokenType::FALSE:
      return Value(false);
    case TokenType::NUMBER:
      return Value(std::stod(std::string(tok.asString())));
    case TokenType::STRING:
      return Value(std::make_shared<String>(tok.asString()));
    case TokenType::NOLL:
      return Value(ValueType::nNoll);
    default:
      UNREACHABLE();
  }
  return Value();
}

void Parser::unexpectedToken(const Token& tok) {
  // can check for specific tokens here for more specific messages
  syntaxError(tok.getLocation(), "Unexpected token: %s\n",
              std::string(tok.asString()).c_str());
}

void Parser::skipBlock() {
  nextToken();
  // Skip block
  while (currentToken() != TokenType::RCB && currentToken() != TokenType::END) {
    // Skip all nested blocks with recursion
    if (currentToken() == TokenType::LCB) skipBlock();

    nextToken();
  }

  match(TokenType::RCB);
}

// Advancing token without reporting error if it was wrong.
bool Parser::match(TokenType expected) {
  if (currentToken() != expected) return false;
  nextToken();
  return true;
}

// Advancing token and reporting error if it was wrong.
void Parser::consume(TokenType expected, const char* error_message) {
  nextToken();
  if (previousToken() != expected) {
    syntaxError(previous_token.getLocation(), error_message);
    if (currentToken() == expected) nextToken();
  }
}

void Parser::expect(TokenType expected, const char* error_message) {
  if (currentToken() != expected)
    syntaxError(current_token.getLocation(), error_message);
}

// automatic semicolon "insertion"
void Parser::expectEndOfStatement(const char* error_message) {
  if (currentToken() == TokenType::SEMICOLON) {
    nextToken();
    return;
  }

  if (current_token.hadNewlineBefore() || currentToken() == TokenType::END ||
      currentToken() == TokenType::RCB) {
    return;
  }

  // if this is reached, an invalid end of statement token has been found
  syntaxError(current_token.getLocation(), error_message);
}

/* ------------- Expression evaluation --------------- */

ExpressionPtr Parser::parseExpression(int precedence = 0) {
  // prefix
  ExpressionPtr expr = parseUnaryPrefixOperation();

  if (expr == nullptr) {
    if (previousToken() == TokenType::END) {
      syntaxError(previous_token.getLocation(),
                  "Unexpectedly reached end of source");
    } else {
      unexpectedToken(previous_token);
      nextToken();
    }
    // Consider just returning nullptr?
    return std::make_unique<NullExpression>();
  }
  // infix / postfix
  while (precedence < Token::precedence(currentToken())) {
    expr = parseBinaryOperation(expr);
  }
  return expr;
}

ExpressionPtr Parser::parseUnaryPrefixOperation() {
  nextToken();
  switch (previousToken()) {
    // Primary expression
    case TokenType::NUMBER:
    case TokenType::STRING:
    case TokenType::NOLL:
    case TokenType::TRUE:
    case TokenType::FALSE:
      return std::make_unique<Literal>(previous_token.getLocation(),
                                       constructValue(previous_token));
    case TokenType::FUNCTION: {
      std::string_view name;
      // Anonymous function with name
      if (currentToken() == TokenType::SYMBOL) {
        name = current_token.asString();
        nextToken();
        // Without name
      } else
        name = "@Anonymous";
      return parseFunctionLiteral(name, FunctionType::anonymous);
    }
    case TokenType::LCB:
      return parseArrayLiteral();
    case TokenType::SYMBOL:
      return std::make_unique<Identifier>(previous_token);
    // Unary prefix operators
    case TokenType::ADD:
      // Just ignore unary add, it has no effect
      return parseExpression(15);
    case TokenType::SUB:
    case TokenType::NOT:
    case TokenType::INCR:
    case TokenType::DECR:
    case TokenType::NEW:
      return std::make_unique<UnaryOperation>(
          parseExpression(15), previous_token, false /* prefix */);
    // Parenthesized expr
    case TokenType::LPAREN: {
      auto expr = parseExpression();
      consume(TokenType::RPAREN, "Expected ')' for end of input.");
      return expr;
    }
    default:
      return nullptr;
  }
}

ExpressionPtr Parser::parseBinaryOperation(ExpressionPtr& left) {
  TokenType type = currentToken();
  Token op = current_token;
  nextToken();  // operator
  if (Token::isAssignOp(type)) {
    return std::make_unique<Assignment>(left, op, parseExpression());
  } else if (Token::isBinaryOp(type)) {
    // 1 if right associative, 0 if left.
    // AND and OR are constructed as right associative so that when
    // left operand is false, the entire right operand chain can be short
    // circuited. The logical opertors are associative so the evaluated value is
    // of course the same anyway.
    int associativity =
        (type == TokenType::EXP || type == TokenType::AND ||
                 type == TokenType::OR || type == TokenType::ASSIGN
             ? 1
             : 0);
    auto right = parseExpression(Token::precedence(type) - associativity);
    return std::make_unique<BinaryOperation>(left, op, right);
  } else {
    // No binary operator, so must be some postfix operator.
    return parseUnaryPostfixOperation(left, op);
  }
}

ExpressionPtr Parser::parseUnaryPostfixOperation(ExpressionPtr& left,
                                                 const Token& tok) {
  // Unary postfix operators
  switch (tok.type()) {
    case TokenType::AMPERSAND:
      // "&" means it's a async function call (run in seperate thread)
      // Todo, create different AST nodes for concurrent/normal call and handle
      // it here. (or maybe just introdude it as a call type).
      // Currently does the same as a normal call.
      if (peek() == TokenType::LPAREN) {
        nextToken();
        return parseCall(left);
      }
      break;
      // "(" means it's a norma function call
    case TokenType::LPAREN:
      return parseCall(left);
    case TokenType::LSB: {
      // Array indexing [expr]
      auto expr = std::make_unique<ArrayAccess>(left, parseExpression());
      consume(TokenType::LSB, "Expected ']'");
      return expr;
    }
    case TokenType::INCR:
    case TokenType::DECR:
      return std::make_unique<UnaryOperation>(std::move(left), tok,
                                              true /* postfix */);
    default:
      break;
  }
  unexpectedToken(previous_token);
  nextToken();  // maybe problem
  return std::make_unique<NullExpression>();
}

ExpressionPtr Parser::parseCall(ExpressionPtr& left) {
  // parse arguments for the call
  CallPtr call = std::make_unique<Call>(left);
  if (currentToken() != TokenType::RPAREN) {
    do {
      call->addArgument(parseExpression());
    } while (match(TokenType::COMMA));
  }
  consume(TokenType::RPAREN, "Expected ')'");
  return call;
}

FunctionLiteralPtr Parser::parseFunctionLiteral(std::string_view name,
                                                FunctionType type) {
  consume(TokenType::LPAREN, "Expected '('");
  std::vector<Identifier> args;
  if (currentToken() != TokenType::RPAREN) {
    do {
      args.push_back(Identifier(current_token));
      consume(TokenType::SYMBOL, "Not a valid function argument identifier");
    } while (match(TokenType::COMMA));
  }

  consume(TokenType::RPAREN, "Expected ')'");
  BlockPtr function_block = parseBlock();
  return std::make_unique<FunctionLiteral>(type, name, args, function_block);
}

ExpressionPtr Parser::parseArrayLiteral() {
  ArrayLiteralPtr arr = std::make_unique<ArrayLiteral>();
  if (currentToken() != TokenType::RCB) {
    do {
      arr->addElement(parseExpression());
    } while (match(TokenType::COMMA));
  }
  consume(TokenType::RCB, "Expected '}'");
  return arr;
}

/* ----------- Expression end ------------- */

/* ----------- Statements ----------------- */

StatementPtr Parser::parseStatement() {
  switch (currentToken()) {
    case TokenType::LCB:
      return parseBlock();
    case TokenType::WHILE:
      return parseWhileStatement();
    case TokenType::IF:
      return parseIfStatement();
    case TokenType::PRINT:
      return parseSingleExpressionStatement<PrintStatement>();
    case TokenType::RETURN:
      return parseSingleExpressionStatement<ReturnStatement>();
    case TokenType::SYMBOL:
      if (previousToken() == TokenType::FUNCTION) {
        nextToken();
        return std::make_unique<ExpressionStatement>(parseFunctionLiteral(
            previous_token.asString(), FunctionType::named));
      }
      break;
    case TokenType::FUNCTION:
      // If there's no symbol it's anonymous, which is handled in
      // parseExpression()
      if (peek() == TokenType::SYMBOL) return parseFunctionDeclaration();
      break;
    default:
      break;
  }
  auto expr = std::make_unique<ExpressionStatement>(parseExpression());
  expectEndOfStatement("Expected end of expression statement");
  return expr;
}

void Parser::addStatement(BlockPtr& block) {
  CHECK(block != nullptr);
  StatementPtr stmt = parseStatement();
  CHECK(stmt != nullptr);
  if (stmt != nullptr) {
    if (stmt->isFunctionDeclaration())
      // Add the declaration.
      block->addDeclaration(stmt);
    else
      block->addStatement(stmt);
  }
}

BlockPtr Parser::parseBlock() {
  consume(TokenType::LCB, "Expected { for start of block.");
  auto block = std::make_unique<Block>();
  while (currentToken() != TokenType::RCB && currentToken() != TokenType::END) {
    addStatement(block);
  }
  consume(TokenType::RCB, "Expected } after block.");
  return block;
}

StatementPtr Parser::parseIfStatement() {
  nextToken();  // if
  consume(TokenType::LPAREN, "Expected ( after if keyword");
  auto condition = parseExpression();
  consume(TokenType::RPAREN, "Expected ) before end of input");
  BlockPtr if_block = parseBlock();
  BlockPtr else_block = nullptr;
  if (match(TokenType::ELSE)) {
    else_block = parseBlock();
  }
  return std::make_unique<IfStatement>(condition, if_block, else_block);
}

StatementPtr Parser::parseWhileStatement() {
  nextToken();  // while
  consume(TokenType::LPAREN, "Expected ( after while keyword");
  auto condition = parseExpression();
  consume(TokenType::RPAREN, "Expected ) before end of input");
  BlockPtr block = parseBlock();
  return std::make_unique<WhileStatement>(condition, block);
}

// print/return
template <class T>
StatementPtr Parser::parseSingleExpressionStatement() {
  nextToken();  // keyword
  auto stmt = std::make_unique<T>(parseExpression());
  expectEndOfStatement("Expected end of statement");
  return stmt;
}

StatementPtr Parser::parseFunctionDeclaration() {
  nextToken();  // function
  // Expect but don't consume, symbol is needed when function literal is parsed
  expect(TokenType::SYMBOL, "Expected function name.");
  return std::make_unique<FunctionDeclaration>(current_token);
}
}  // namespace linaro