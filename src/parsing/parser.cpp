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

Parser::Parser(const Lexer& lex) : m_lex(lex) {
  // Fill buffer with the initial 6 tokens
  for (int i = 0; i < buffer_size; i++) buffer[i] = m_lex.nextToken();
  current_token = buffer[0];
  nextToken();
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
      case TokenType::FUNCTION:
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
      return Value(std::stod(tok.asString()));
    case TokenType::STRING:
      return Value(tok.asString());
    case TokenType::NOLL:
      return Value(ValueType::nNoll);
    default:
      UNREACHABLE();
  }
  return Value();
}

void Parser::unexpectedToken(const Token& tok) {
  // can check for specific tokens here for more specific messages
  syntaxError(tok.getLocation(), "Unexpected token: %s", tok.asString());
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
  if (current_token.Type() != expected) return false;
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
  auto left = parseUnaryPrefixOperation();

  if (!left) {
    if (previousToken() == TokenType::END) {
      syntaxError(previous_token.getLocation(),
                  "Unexpectedly reached end of source");
    } else {
      unexpectedToken(previous_token);
      nextToken();
    }

    return std::make_unique<NullExpression>();
  }

  // infix / postfix
  while (precedence < Token::getPrecedence(currentToken())) {
    left = parseBinaryOperation(std::move(left));
  }
  return left;
}

ExpressionPtr Parser::parseUnaryPrefixOperation() {
  Token temp = current_token;
  nextToken();
  switch (temp.type()) {
      // primary expression
    case TokenType::NUMBER:
    case TokenType::STRING:
    case TokenType::NOLL:
    case TokenType::TRUE:
    case TokenType::FALSE:
      return std::make_unique<Expression>(
          Literal(temp.getLocation(), constructValue(temp)));
    case TokenType::SYMBOL:
      return std::make_unique<Expression>(Identifier(temp));
    // function expression
    case TokenType::FUNCTION:
      return parseFunctionLiteral("Anonymous", FunctionType::anonymous);
    case TokenType::THIS:
      return make_shared<This>(temp);
    case TokenType::SUPER:
      return make_shared<Super>(temp);
    // unary prefix operators
    case TokenType::ADD:
      return ParseExpression(
          15);  // (what did i think here? might be correct tho)
    case TokenType::SUB:
    case TokenType::NOT:
    case TokenType::INCR:
    case TokenType::DECR:
    case TokenType::NEW:
      return make_shared<UnaryOperation>(temp, ParseExpression(15),
                                         false /* prefix */);
    // parenthesized expr
    case LPAREN: {
      auto expr = ParseExpression();
      Consume(RPAREN, "Expected ')' for end of input.");
      return expr;
    }
    default:
      return nullptr;
  }
}

Expr Parser::ParseBinaryOperation(Expr left) {
  Token temp_prev = previous_token;
  Token temp = current_token;
  TokenType type = current_token.Type();
  NextToken();  // operator
  if (Token::IsAssignOp(type)) {
    return ParseAssignment(left, temp);
  } else if (Token::IsBinaryOp(type)) {
    // 1 if right associative, 0 if left.
    // AND and OR are constructed as right associative so that when
    // left operand is false, the entire right operand chain can be short
    // circuited. The logical opertors are associative so the evaluated value is
    // of course the same anyway.
    int associativity =
        type == EXP || type == AND || type == OR || type == ASSIGN ? 1 : 0;
    auto right = ParseExpression(Token::Precedence(type) - associativity);

    /*
    Right operand of . operator has to be an identifier.
    Left operand of . operator can be a
    function call, identifier or another binary operation with . as operator

    Note that if it's a binary operation, the UnexpectedToken report may be a
    bit off, might fix.
    */

    if (type == PERIOD) {
      if (!right->IsIdentifier()) {
        UnexpectedToken(previous_token);
      } else if (!left->IsIdentifier() && !left->IsBinaryOperation() &&
                 !left->IsThis() && !left->IsSuper() && !left->IsCall()) {
        UnexpectedToken(temp_prev);
      }
    }
    TransformBinOpWithNumberLiterals(left, right, temp, Loc());
    return left;
  }

  // unary postfix operators
  switch (temp.Type()) {
    // call indicated by '('
    case LPAREN: {
      CallType type;
      if (left->IsIdentifier()) {
        type = CallType::NAMED;
      } else if (left->IsBinaryOperation() &&
                 left->AsBinaryOperation()->IsMemberAccess()) {
        type = CallType::METHOD;
      }
      // can be removed
      else if (left->IsUnaryOperation() &&
               left->AsUnaryOperation()->IsNewOperator()) {
        type = CallType::NEW_OBJ;
      }
      // todo: super and this call
      else if (left->IsSuper()) {
        type = CallType::SUPER;
      } else if (left->IsSuper()) {
        type = CallType::THIS;
      } else {
        // If this is reached, it's an anonymous call [expr]()
        type = CallType::ANONYMOUS;
      }
      return ParseCall(left, type);
    }
    case RSB:
      // TODO: e.g. array indexing []
      break;
    case INCR:
    case DECR:
      return make_shared<UnaryOperation>(temp, left, true /* postfix */);
  }

  UnexpectedToken(previous_token);
  NextToken();  // THIS COULD BE A PROBLEM (if a weird error happens, check
                // this)
  return make_shared<NullExpression>();
}

Expr Parser::ParseCall(Expr left, CallType type) {
  /*
  if (!left->IsValidReferenceExpression()) {
          err->ErrorAt(current_token.Location(), "Cannot be called");
          while (current_token.Type() != RPAREN) NextToken();
          NextToken();
          return make_shared<NullExpression>();
  }
  */
  // parse arguments for the call
  std::vector<Expr> args;
  if (current_token.Type() != RPAREN) {
    do {
      args.push_back(ParseExpression());
    } while (Match(COMMA));
  }

  Consume(RPAREN, "Expected ')'");
  return make_shared<Call>(Call(left, args, type));
}

Expr Parser::ParseAssignment(Expr left, const Token& tok) {
  // could potentially check LHS here for lvalue.
  // But might just do it during AST visiation?
  Expr expr = ParseExpression();
  // ExpectEndOfStatement("Expected end of input"); is checked already in parse
  // statement right?
  return make_shared<Assignment>(left, tok, expr);
}

Fn Parser::ParseFunctionLiteral(std::string name, FunctionType type) {
  Consume(LPAREN, "Expected '('");
  std::vector<Identifier> args;
  if (current_token.Type() != RPAREN) {
    do {
      args.push_back(Identifier(current_token));
      Consume(SYMBOL, "Not a valid function argument identifier");
    } while (Match(COMMA));
  }

  Consume(RPAREN, "Expected ')'");
  auto function_block = ParseBlock();
  return make_shared<FunctionLiteral>(name, args, function_block, type);
}

/* ----------- Expression end ------------- */

/* ----------- Statements ----------------- */

Stmt Parser::parse() {
  auto program = std::make_unique<FunctionLiteral>(
      FunctionType::top_level, "@main",
      {Token(TokenType::SYMBOL, "argc"), Token(TokenType::SYMBOL, "argv")},
      main_body);
  while (currentToken() != TokenType::END) {
    Stmt st = parseStatement();
    if (st != nullptr) {
      program->addStatement(st);
    }
  }
  return program;
}

StatementPtr Parser::parseStatement() {
  StatementPtr stmt;
  switch (currentToken()) {
    case TokenType::LCB: {
      stmt = std::make_unique<Block>();
      parseBlock(stmt);
      break;
    }
    case TokenType::WHILE: {
      stmt = std::make_unique<WhileStatement>();
      parseWhileStatement(stmt);
      break;
    }
    case TokenType::IF: {
      stmt = std::make_unique<IfStatement>();
      parseIfStatement(stmt);
      break;
    }
    case TokenType::PRINT: {
      stmt = std::make_unique<PrintStatement>();
      parseSingleExpressionStatement<PrintStatement>(stmt);
      break;
    }
    case TokenType::RETURN {
      stmt = std::make_unique<ReturnStatement>();
      ParseSingleExpressionStatement<ReturnStatement>(stmt);
      break;
    }:
    case TokenType::FUNCTION: {
      stmt = std::make_unique<Functiondeclaration>(stmt);
      // If there's no symbol it's anonymous, which is handled in
      // ParseExpression()
      if (peek() == TokenType::SYMBOL)
        parseFunctionDeclaration(FunctionType::named);
      break;
    }

    default:
      // if no valid start of statement then it's an expression
      stmt = std::make_unique<ExpressionStatement>(parseExpression());
      expectEndOfStatement("Expected end of expression statement");
  }
  return stmt;
}

void Parser::parseBlock(BlockPtr& block) {
  Consume(LCB, "Expected { for start of block.");
  while (currentToken() != TokenType::RCB && currentToken() != TokenType::END) {
    if (st != nullptr) block->addStatement(st);
  }
  Consume(RCB, "Expected } after block.");
}

Stmt Parser::ParseFunctionDeclaration(FunctionType type) {
  NextToken();  // function
  Token name = current_token;
  Consume(SYMBOL, "Expected function name.");
  auto fn = ParseFunctionLiteral(name.getValue().AsString(), type);
  return make_shared<FunctionDeclaration>(name, fn);
}

Stmt Parser::ParseIfStatement() {
  NextToken();  // if
  Consume(LPAREN, "Expected ( after if keyword");
  auto condition = ParseExpression();
  Consume(RPAREN, "Expected ) before end of input");
  auto if_block = ParseBlock();
  std::shared_ptr<Block> else_block = nullptr;
  if (Match(ELSE)) {
    else_block = ParseBlock();
  }
  return make_shared<IfStatement>(condition, if_block, else_block);
}

Stmt Parser::ParseWhileStatement() {
  NextToken();  // while
  Consume(LPAREN, "Expected ( after while keyword");
  auto condition = ParseExpression();
  Consume(RPAREN, "Expected ) before end of input");
  auto block = ParseBlock();
  return make_shared<WhileStatement>(condition, block);
}

// print/return
template <class T>
Stmt Parser::ParseSingleExpressionStatement() {
  Token temp = current_token;
  NextToken();  // keyword
  auto st = make_shared<T>(temp, ParseExpression());
  ExpectEndOfStatement("Expected end of statement");
  return st;
}

Stmt Parser::ParseVariableDeclaration() {
  NextToken();  // var/field
  Token name = current_token;
  if (Peek() != ASSIGN) {
    Consume(SYMBOL, "Expected a name for variable declaration");
    ExpectEndOfStatement("Expected end of variable declaration");
  }
  return make_shared<VariableDeclaration>(name);
}

Stmt Parser::ParseClassDeclaration() {
  NextToken();  // class
  auto symbol = Identifier(current_token);
  Consume(SYMBOL, "Expected class name");

  Expr super = make_shared<NullExpression>();
  if (current_token.Type() == INHERITS) {
    NextToken();  // inherits
    super = make_shared<Identifier>(current_token);
    Consume(SYMBOL, "Expected super name");
  }

  Consume(LCB, "Expected '{'");
  auto class_block = make_shared<Block>();
  bool needs_constructor = true;
  // when class block is visited, the constructor function BytecodeChunk will be
  // added to the code stack!!!!
  while (current_token.Type() != RCB && current_token.Type() != END) {
    switch (current_token.Type()) {
      case METHOD:
        class_block->AddStatement(
            ParseFunctionDeclaration(FunctionType::method));
        break;
      case CONSTRUCTOR: {
        needs_constructor = false;
        NextToken();  // constructor
        auto fn =
            ParseFunctionLiteral(symbol.Name(), FunctionType::constructor);
        class_block->AddStatement(
            make_shared<FunctionDeclaration>(symbol.getToken(), fn));
      } break;
      case FIELD:
        class_block->AddStatement(ParseVariableDeclaration());
        // could be an issue if theres a symbol at start of next statement?
        // (cant be the case in class i guess?)
        if (current_token.Type() == SYMBOL) {
          Token temp = current_token;  // symbol
          NextToken();
          Token assign_tok = current_token;
          Consume(ASSIGN,
                  "Expected '=' token for direct intialization in class");
          Expr assign =
              ParseAssignment(make_shared<Identifier>(temp), assign_tok);
          class_block->AddStatement(make_shared<ExpressionStatement>(assign));
        }
        break;
      default:
        err->ErrorAt(current_token.getLocation(),
                     "Invalid start of statement in class block");
        // Synchronize();
        break;
    }
  }

  Consume(RCB, "Expected '}'");

  if (needs_constructor) {
    // this can be done better, it's essentially an empty method with the name
    // of the class.
    class_block->AddStatement(make_shared<FunctionDeclaration>(
        symbol.getToken(),
        make_shared<FunctionLiteral>(symbol.Name(), std::vector<Identifier>(),
                                     make_shared<Block>(),
                                     FunctionType::constructor)));
  }

  return make_shared<ClassDeclaration>(symbol, class_block, super);
}

}  // namespace linaro