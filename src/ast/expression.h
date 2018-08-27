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
                  std::vector<Identifier> args, BlockPtr block)
      : Expression(nFunctionLiteral),
        m_type(type),
        m_function_name(name),
        m_args(args),
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
  BinaryOperation(ExpressionPtr left, const Token& op, ExpressionPtr right)
      : Expression(nBinaryOperation),
        m_left(std::move(left)),
        m_op(op),
        m_right(std::move(right)) {}

  bool isValidReferenceExpression() override;
  bool isLogicalOperation() { return Token::isLogicalOp(m_op.type()); }
  bool isMemberAccess() { return m_op.type() == Token::PERIOD; }
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
  UnaryOperation(const Token& op, ExpressionPtr operand, bool is_postfix)
      : Expression(nUnaryOperation),
        m_op(op),
        m_operand(std::move(operand)),
        m_is_postfix(is_postfix) {}

  const Token& op() const { return m_op; }
  Expression* operand() const { return m_operand.get(); }
  bool isNewOperator() { return m_op.type() == Token::NEW; }

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
  const Token m_op;
  ExpressionPtr m_operand;
  bool m_is_postfix;
};

class Assignment : public Expression {
 public:
  Assignment(ExpressionPtr target, Token op, ExpressionPtr right)
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

enum class CallType {
  /*
  Named:  Tries to find function symbol and calls it. If not found then
          it was an anonymous call to a single identifier, in which case
                  the TOS is called.
  Syntax: [name]([args])
  */
  NAMED,

  /*
  Super:  Call constructor of superclass. Can only be called inside
              of the dervied classes constructor.
  Syntax: super([args])
  */
  SUPER,

  /*
  This:   Call constructor of own class. Can only be called inside
              of constructor.
  Syntax: this([args])
  */
  THIS,

  /*
  Anms:   Calls whatever is put on top of the operand stack by [expr].
                  Runtime error if it's not a function object. Note that
                  [expr] could be an identifier, which means it will be a
                  named call instead. (Read descriptor of NAMED).
  Syntax: [expr]([args])
  */
  ANONYMOUS,

  /*
  Method: Calls a method of some class instance.
  Syntax: [expr].[symbol]([args])
  Issue:	x.b() could be for method "b" or field "b" containing an
  anonymous function. This will be resolved at runtime.
  */
  METHOD,

  /*
  New:   Constructs a new class instance of type [class_name], calls
         it's constructor and puts the initialized instance on TOS.
  Syntax: new [class_name]([args])
  */
  NEW_OBJ

};

class Call : public Expression {
 public:
  Call(CallType type, ExpressionPtr caller)
      : Expression(nCall), m_type(type), m_caller(std::move(caller)) {}

  Call* asCall() { return this; }
  const auto& arguments() const { return m_arguments; }
  Expression* caller() const { return m_caller.get(); }

  void setCallType(CallType type) { m_type = type; }
  void addArgument(ExpressionPtr arg) { m_arguments.push_back(std::move(arg)); }
  bool isNamedCall() const { return m_type == CallType::NAMED; }
  bool isSuperCall() const { return m_type == CallType::SUPER; }
  bool isThisCall() const { return m_type == CallType::THIS; }
  bool isMethodCall() const { return m_type == CallType::METHOD; }
  bool isAnonymousCall() const { return m_type == CallType::ANONYMOUS; }
  bool isNewCall() const { return m_type == CallType::NEW_OBJ; }
  bool isValidReferenceIdentifier();

  void visit(NodeVisitor& v) override { v.visitCall(*this); }
  void printNode() const override {
    std::cout << "FunctionCall(";
    m_caller->printNode();
    if (!m_arguments.empty()) {
      std::cout << ", args: ";
      for (auto& s : m_arguments) {
        s->printNode();
        std::cout << ", ";
      }
    }
    std::cout << ")";
  }

 private:
  CallType m_type;
  ExpressionPtr m_caller;
  std::vector<ExpressionPtr> m_arguments;
};

}  // namespace linaro
#endif  // EXPRESSION_H
