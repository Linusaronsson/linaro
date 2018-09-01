#ifndef STATEMENT_H
#define STATEMENT_H

#include <memory>

#include "../parsing/token.h"
#include "ast.h"
#include "expression.h"

namespace Linaro {
class Statement : public Node {
 protected:
  Statement(NodeType type) : Node(type) {}
};

// Each compound statement corresponds to a block with a scope.
// Blocks are opened and closed by '{' and '}'.
class Block : public Statement {
 public:
  Block() : Statement(nBlock) {}

  const auto& getStatements() const { return statements; }
  const auto& getDeclarations() const { return declarations; }
  bool isEmpty() const { return statements.empty(); }

  void addStatement(StatementPtr& statement) {
    statements.push_back(std::move(statement));
  }

  void addDeclaration(StatementPtr& declaration) {
    declarations.push_back(std::move(declaration));
  }

  void visit(NodeVisitor& v) override { v.visitBlock(*this); }
  void printNode() const override {
    std::cout << "Block: {\n--- Declarations ---\n";
    for (const auto& stat : declarations) {
      stat->printNode();
      std::cout << '\n';
    }

    std::cout << "--- Statements ---\n";
    for (const auto& stat : statements) {
      stat->printNode();
      std::cout << '\n';
    }
    std::cout << "} // End of Block\n";
  }

 private:
  // Declarations are visited first for forward ref.
  // For named function (because they should be forward declared):
  // Visit a FunctionDeclaration, which defines it as a FunctionSymbol
  // in the SymbolTable. It will save it's index into the constant pool, and
  // put a dummy Value in its place.
  // Later when the FunctionLiteral is visited, we look up the name (which is
  // also saved in the FunctionLiteral) in the SymbolTable to get the index and
  // then insert the function literal at this index. So note this:
  // VariableDeclaration and FunctionDeclarations are visited first, but NOT the
  // FunctionLiteral.
  std::vector<StatementPtr> declarations;
  std::vector<StatementPtr> statements;
};

class ExpressionStatement : public Statement {
 public:
  ExpressionStatement(ExpressionPtr expr)
      : Statement(nExpressionStatement), m_expr(std::move(expr)) {}

  Expression* expr() const { return m_expr.get(); }
  void addExpression(ExpressionPtr& expr) { m_expr = std::move(expr); }
  void visit(NodeVisitor& v) override { v.visitExpressionStatement(*this); }
  void printNode() const override { m_expr->printNode(); }

 private:
  ExpressionPtr m_expr;
};

class ReturnStatement : public Statement {
 public:
  ReturnStatement(ExpressionPtr expr)
      : Statement(nReturnStatement), return_expr(std::move(expr)) {}

  Expression* expr() const { return return_expr.get(); }

  void visit(NodeVisitor& v) override { v.visitReturnStatement(*this); }
  void printNode() const override {
    std::cout << "Return(";
    return_expr->printNode();
    std::cout << ")";
  }

 private:
  ExpressionPtr return_expr;
};

class PrintStatement : public Statement {
 public:
  PrintStatement(ExpressionPtr expr)
      : Statement(nPrintStatement), print_expr(std::move(expr)) {}

  Expression* expr() const { return print_expr.get(); }

  void visit(NodeVisitor& v) override { v.visitPrintStatement(*this); }
  void printNode() const override {
    std::cout << "Print(";
    print_expr->printNode();
    std::cout << ")";
  }

 private:
  ExpressionPtr print_expr;
};

class FunctionDeclaration : public Statement {
 public:
  FunctionDeclaration(const Token& symbol)
      : Statement(nFunctionDeclaration), m_symbol(symbol) {}

  const Token& symbol() const { return m_symbol; }
  const Location& loc() const { return m_symbol.getLocation(); }

  void visit(NodeVisitor& v) override { v.visitFunctionDeclaration(*this); }
  void printNode() const override {
    std::cout << "FunctionDeclaration(" << m_symbol.asString() << ")";
  }

 private:
  Token m_symbol;
};

class IfStatement : public Statement {
 public:
  IfStatement(ExpressionPtr& condition, BlockPtr& then_block,
              BlockPtr& else_block)
      : Statement(nIfStatement),
        m_condition(std::move(condition)),
        m_then_block(std::move(then_block)),
        m_else_block(std::move(else_block)) {}

  bool hasThenBlock() const { return !m_then_block->isEmpty(); }
  bool hasElseBlock() const {
    return m_else_block != nullptr && !m_then_block->isEmpty();
  }

  Expression* expr() const { return m_condition.get(); }
  Block* ifBlock() const { return m_then_block.get(); }
  Block* elseBlock() const { return m_else_block.get(); }

  void visit(NodeVisitor& v) override { v.visitIfStatement(*this); }
  void printNode() const override {
    std::cout << "If_Statement[(";
    m_condition->printNode();
    std::cout << ") {\n";
    m_then_block->printNode();
    std::cout << "}";
    if (m_else_block) {
      std::cout << "else {";
      m_else_block->printNode();
      std::cout << "}";
    }
    std::cout << "]";
  }

 private:
  ExpressionPtr m_condition;
  BlockPtr m_then_block;
  BlockPtr m_else_block;
};

class WhileStatement : public Statement {
 public:
  WhileStatement(ExpressionPtr& boolean_expr, BlockPtr& while_block)
      : Statement(nWhileStatement),
        m_boolean_expr(std::move(boolean_expr)),
        m_while_block(std::move(while_block)) {}

  Expression* expr() const { return m_boolean_expr.get(); }
  Block* whileBlock() const { return m_while_block.get(); }

  void visit(NodeVisitor& v) override { v.visitWhileStatement(*this); }
  void printNode() const override {
    std::cout << "While_Statement[(";
    m_boolean_expr->printNode();
    std::cout << ") {";
    m_while_block->printNode();
    std::cout << "}]";
  }

 private:
  ExpressionPtr m_boolean_expr;
  BlockPtr m_while_block;
};

}  // namespace linaro
#endif  // STATEMENT_H
