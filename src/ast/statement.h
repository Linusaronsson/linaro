#ifndef STATEMENT_H
#define STATEMENT_H

#include <memory>

#include "../parsing/token.h"
#include "ast.h"
#include "expression.h"

namespace linaro {
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

  void addStatement(StatementPtr statement) {
    statements.push_back(std::move(statement));
  }

  void addDeclaration(StatementPtr declaration) {
    declarations.push_back(std::move(declaration));
  }

  void visit(NodeVisitor& v) override { v.visitBlock(*this); }
  void printNode() const override {
    std::cout << "Block[\nDeclarations:\n";
    for (const auto& stat : declarations) {
      stat->printNode();
      std::cout << '\n';
    }

    std::cout << "Statements:\n";
    for (const auto& stat : statements) {
      stat->printNode();
      std::cout << '\n';
    }
    std::cout << "] //end of compound\n";
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
  void visit(NodeVisitor& v) override { v.visitExpressionStatement(*this); }
  void printNode() const override {
    std::cout << "Expr(";
    m_expr->printNode();
    std::cout << ")";
  }

 private:
  ExpressionPtr m_expr;
};

class EmptyStatement : public Statement {
 public:
  EmptyStatement() : Statement(nEmptyStatement) {}
  void visit(NodeVisitor& v) override { v.visitEmptyStatement(*this); }
  void printNode() const override { std::cout << "EmptyStatement\n"; }
};

class ClassDeclaration : public Statement {
 public:
  ClassDeclaration(const Identifier& this_, BlockPtr class_block,
                   ExpressionPtr superclass)
      : Statement(nClassDeclaration),
        m_this(this_),
        m_class_block(std::move(class_block)),
        m_superclass(std::move(superclass)) {}

  Expression* super() const { return m_superclass.get(); }
  const Identifier& getIdentifier() const { return m_this; }
  Block* getBlock() const { return m_class_block.get(); }

  void visit(NodeVisitor& v) override { v.visitClassDeclaration(*this); }
  void printNode() const override {
    //    std::cout << "Class(";
    //    this_name.printNode();
    //    if (!m_superclass->isNullExpression()) {
    //      std::cout << ", ";
    //      std::cout << "Inherits from: ";
    //      m_superclass->PrintNode();
    //    }
    //    std::cout << ") { " << std::endl;
    //    m_class_block->PrintNode();
    //    std::cout << "} //End of class block" << std::endl;
  }

 private:
  const Identifier m_this;
  BlockPtr m_class_block;
  // identifier or null expression if there is none
  ExpressionPtr m_superclass;
};

class FunctionDeclaration : public Statement {
 public:
  FunctionDeclaration(const Token& name)
      : Statement(nFunctionDeclaration), m_function_name(name) {}

  const Location& loc() const { return m_function_name.getLocation(); }
  std::string_view name() const { return m_function_name.asString(); }

  void visit(NodeVisitor& v) override { v.visitFunctionDeclaration(*this); }
  void printNode() const override {
    std::cout << "FunctionDeclaration(";
    m_function_name.asString();
    std::cout << ")";
  }

 private:
  // for id location
  Token m_function_name;
  // Big note: The function literal is now removed from here. It should simply
  // be parsed seperately and put in a expression statement. This means we can
  // visit all declarations and therefor deal with the forward declaration
  // problem. The function literal is then visited as a seperate statement. The
  // first thing that is done when visiting the FunctionLiteral is to look up
  // the name of the function in the symbol table, and there it will find THIS
  // declaration. From there, it gets it's index where it should put itself into
  // the const pool (currently a dummy value).
  // FunctionLiteralPtr m_func;
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

class VariableDeclaration : public Statement {
 public:
  VariableDeclaration(const Token& symbol)
      : Statement(nVariableDeclaration), m_symbol(symbol) {}

  VariableDeclaration* asVariableDeclaration() { return this; }
  const Token& symbol() const { return m_symbol; }
  const Location& loc() const { return m_symbol.getLocation(); }

  void visit(NodeVisitor& v) override { v.visitVariableDeclaration(*this); }
  void printNode() const override {
    std::cout << "VarDecl(";
    m_symbol.asString();
    std::cout << ")";
  }

 private:
  Token m_symbol;
};

class IfStatement : public Statement {
 public:
  IfStatement(ExpressionPtr condition, BlockPtr then_block,
              BlockPtr else_block = nullptr)
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
  WhileStatement(ExpressionPtr boolean_expr, BlockPtr while_block)
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
