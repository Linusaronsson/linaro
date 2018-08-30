#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>

#include "../parsing/token.h"
#include "ast.h"

namespace linaro {
class Expression : public Node {
 public:
  virtual bool isValidReferenceExpression() { return false; }
  bool isFalse();
  bool isTrue();
  bool isNullLiteral();
  bool isNumberLiteral();
  bool isStringLiteral();
  bool toBooleanIsTrue();
  bool toBooleanIsFalse();

 protected:
  Expression(NodeType type) : Node(type) {}
};

// Literal is for: null, true, false, number, string
class Literal : public Expression {
 public:
  Literal(const Location& loc, const Value& val)
      : Expression(nLiteral), m_loc(loc), m_val(val) {}

  // Returns value. The value
  const Value& value() const { return m_val; }
  const Location& loc() const { return m_loc; }

  bool isFalse() const { return !m_val.asBoolean(); }
  bool isTrue() const { return !m_val.asBoolean(); }

  void visit(NodeVisitor& v) override { v.visitLiteral(*this); }
  void printNode() const override {
    std::cout << "Literal(" << m_val.asString() << ")";
  }

 private:
  Location m_loc;
  Value m_val;
};

// null expressions are returned if an expression was incorrect syntactically.
// null exression is just a conveniance class that make IsValidReference work
// (always false ofc) even if the expression was null. The other option is to
// return a raw nullptr which we then have to check every time, which will
// likely get annoying
class NullExpression : public Expression {
 public:
  NullExpression() : Expression(nNullExpression) {}
  void visit(NodeVisitor& v) override { v.visitNullExpression(*this); }
  void printNode() const override { std::cout << "NullExpression\n"; }
};

// Named simply means it was defined with a name, but will behave the same
// as an anonymous function.
enum class FunctionType { anonymous, named, method, constructor, top_level };

class FunctionLiteral : public Expression {
 public:
  FunctionLiteral(FunctionType type, std::string_view name,
                  const std::vector<Identifier>& args, BlockPtr& block)
      : Expression(nFunctionLiteral),
        m_type(type),
        m_function_name(name),
        m_args(std::move(args)),
        m_function_block(std::move(block)) {}

  void addArgument(const Identifier& id) { m_args.push_back(id); }

  Block* block() const { return m_function_block.get(); }
  bool isAnonymous() const { return m_type == FunctionType::anonymous; }
  bool isNamed() const { return m_type == FunctionType::named; }
  bool isMethod() const { return m_type == FunctionType::method; }
  bool isConstructor() const { return m_type == FunctionType::constructor; }
  bool isTopLevel() const { return m_type == FunctionType::top_level; }
  const auto& args() const { return m_args; }
  int numArgs() const { return m_args.size(); }
  std::string_view name() const { return m_function_name; }

  void visit(NodeVisitor& v) override { v.visitFunctionLiteral(*this); }
  void printNode() const override;

 private:
  FunctionType m_type;
  std::string_view m_function_name;
  std::vector<Identifier> m_args;
  BlockPtr m_function_block;
};

class ArrayLiteral : public Expression {
 public:
  ArrayLiteral() : Expression(nArrayLiteral) {}

  void addElement(ExpressionPtr element) {
    m_elements.push_back(std::move(element));
  }

  const auto& elements() const { return m_elements; }

  void visit(NodeVisitor& v) override { v.visitArrayLiteral(*this); }
  void printNode() const override {
    std::cout << "ArrayLiteral(";
    if (!m_elements.empty()) {
      std::cout << "Args: ";
      for (auto it = m_elements.begin(); it != m_elements.end(); it++) {
        it->get()->printNode();
        if (std::next(it) != m_elements.end()) std::cout << ", ";
      }
    }
    std::cout << ")";
  }

 private:
  std::vector<ExpressionPtr> m_elements;
};

class ArrayAccess : public Expression {
 public:
  ArrayAccess(ExpressionPtr& target, ExpressionPtr index)
      : Expression(nArrayAccess),
        m_target(std::move(target)),
        m_index(std::move(index)) {}

  Expression* index() const { return m_index.get(); }

  void visit(NodeVisitor& v) override { v.visitArrayAccess(*this); }
  void printNode() const override {
    std::cout << "ArrayAccess(Target: ";
    m_target->printNode();
    std::cout << ", Index: ";
    m_index->printNode();
    std::cout << ')';
  }

 private:
  ExpressionPtr m_target;
  ExpressionPtr m_index;
};

class Identifier : public Expression {
 public:
  Identifier(const Token& tok) : Expression(nIdentifier), m_tok(tok) {}

  bool isValidReferenceExpression() override { return true; }

  std::string_view name() const { return m_tok.asString(); }
  const Token& getToken() const { return m_tok; }
  const Location& getLocation() const { return m_tok.getLocation(); }

  void visit(NodeVisitor& v) override { v.visitIdentifier(*this); }
  void printNode() const override { std::cout << "Symbol(" << name() << ")"; }

 private:
  const Token m_tok;
};

class BinaryOperation : public Expression {
 public:
  BinaryOperation(ExpressionPtr& left, const Token& op, ExpressionPtr& right)
      : Expression(nBinaryOperation),
        m_left(std::move(left)),
        m_op(op),
        m_right(std::move(right)) {}

  bool isValidReferenceExpression() override;
  bool isLogicalOperation() { return Token::isLogicalOp(m_op.type()); }
  bool isMemberAccess() { return m_op.type() == TokenType::PERIOD; }
  const Token& op() const { return m_op; }

  Expression* leftOperand() const { return m_left.get(); }
  Expression* rightOperand() const { return m_right.get(); }
  void setLeftOperand(ExpressionPtr op) { m_left = std::move(op); }
  void setRightOperand(ExpressionPtr op) { m_right = std::move(op); }

  void visit(NodeVisitor& v) override { v.visitBinaryOperation(*this); }
  void printNode() const override {
    std::cout << "BinOp(";
    m_left->printNode();
    std::cout << ", '" << m_op.asString() << "', ";
    m_right->printNode();
    std::cout << ")";
  }

 private:
  ExpressionPtr m_left;
  const Token m_op;
  ExpressionPtr m_right;
};

class UnaryOperation : public Expression {
 public:
  UnaryOperation(ExpressionPtr operand, const Token& op, bool is_postfix)
      : Expression(nUnaryOperation),
        m_operand(std::move(operand)),
        m_op(op),
        m_is_postfix(is_postfix) {}

  const Token& op() const { return m_op; }
  Expression* operand() const { return m_operand.get(); }

  //-, +, --, ++, !
  bool isPrefix() { return !m_is_postfix; }

  //--, ++, (), []
  bool isPostfix() { return m_is_postfix; }

  void visit(NodeVisitor& v) override { v.visitUnaryOperation(*this); }
  void printNode() const override {
    if (m_is_postfix) {
      std::cout << "PostfixUnaryOp('";
      m_operand->printNode();
      std::cout << "', " << m_op.asString();
      std::cout << ")";
    } else {
      std::cout << "PrefixUnaryOp('" << m_op.asString() << "', ";
      m_operand->printNode();
      std::cout << ")";
    }
  }

 private:
  ExpressionPtr m_operand;
  const Token m_op;
  bool m_is_postfix;
};

class Assignment : public Expression {
 public:
  Assignment(ExpressionPtr& target, Token op, ExpressionPtr right)
      : Expression(nAssignment),
        m_target(std::move(target)),
        m_op(op),
        m_right(std::move(right)) {}

  const Token& getToken() const { return m_op; }
  Expression* target() const { return m_target.get(); }
  Expression* rightOperand() const { return m_right.get(); }

  void visit(NodeVisitor& v) override { v.visitAssignment(*this); }
  void printNode() const override {
    std::cout << "VarAssignment(";
    m_target->printNode();
    std::cout << ", '";
    std::cout << m_op.asString();
    std::cout << "', ";
    m_right->printNode();
    std::cout << ")";
  }

 private:
  ExpressionPtr m_target;  // must be lvalue
  const Token m_op;
  ExpressionPtr m_right;
};

// If I will need call types in the future, introduce sub classes to Call
// for each type instead. Keep common things in the Call class.
class Call : public Expression {
 public:
  Call(ExpressionPtr& caller)
      : Expression(nCall), m_caller(std::move(caller)) {}

  Call* asCall() { return this; }
  const auto& arguments() const { return m_args; }
  Expression* caller() const { return m_caller.get(); }

  void addArgument(ExpressionPtr arg) { m_args.push_back(std::move(arg)); }
  bool isValidReferenceIdentifier();

  void visit(NodeVisitor& v) override { v.visitCall(*this); }
  void printNode() const override {
    std::cout << "FunctionCall(";
    std::cout << "Caller: ";
    m_caller->printNode();

    if (!m_args.empty()) {
      std::cout << ", Args: ";
      for (auto it = m_args.begin(); it != m_args.end(); it++) {
        it->get()->printNode();
        if (std::next(it) != m_args.end()) std::cout << ", ";
      }
    }
    std::cout << ")";
  }

 private:
  ExpressionPtr m_caller;
  std::vector<ExpressionPtr> m_args;
};

}  // namespace linaro
#endif  // EXPRESSION_H
