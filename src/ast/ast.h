#ifndef AST_H
#define AST_H

#include <iostream>
#include <memory>
#include <string_view>

#include "../parsing/token.h"

namespace Linaro {

#define AST_NODES(T)      \
  /* Expression nodes: */ \
  T(Literal)              \
  T(FunctionLiteral)      \
  T(ArrayLiteral)         \
  T(ArrayAccess)          \
  T(Identifier)           \
  T(BinaryOperation)      \
  T(Assignment)           \
  T(Call)                 \
  T(UnaryOperation)       \
                          \
  /*T(Conditional)*/      \
  T(NullExpression)       \
                          \
  /* Statement nodes:*/   \
  T(ExpressionStatement)  \
  T(Block)                \
  T(ReturnStatement)      \
  T(PrintStatement)       \
  T(FunctionDeclaration)  \
  T(IfStatement)          \
  T(WhileStatement)

// Base class for all nodes
class Node;
// Base class for expressions
class Expression;
// Base class for statements
class Statement;

// Forward declare all node types.
// Typedef std::unique_ptr to all node types.
// E.g: std::unique_ptr<Expression> becomes ExpressionPtr.
#define T(name) \
  class name;   \
  using name##Ptr = std::unique_ptr<name>;
AST_NODES(T)
#undef T

using ExpressionPtr = std::unique_ptr<Expression>;
using StatementPtr = std::unique_ptr<Statement>;
using NodePtr = std::unique_ptr<Node>;

// Interface for a NodeVisitor class.
class NodeVisitor {
 public:
#define T(type) virtual void visit##type(const type& node) = 0;
  AST_NODES(T)
#undef T
};

class Node {
 public:
#define T(type) n##type,
  enum NodeType : uint8_t { AST_NODES(T) };
#undef T

#define T(type)                                       \
  bool is##type() const { return m_type == n##type; } \
  type* as##type();
  AST_NODES(T)
#undef T

  bool isDeclaration() const { return m_type == nFunctionDeclaration; }

  NodeType type() const { return m_type; }
  virtual void visit(NodeVisitor& v) = 0;
  //  virtual const Location& location() const = 0;
  virtual void printNode() const = 0;
  virtual ~Node() {}

 protected:
  Node(NodeType type) : m_type(type) {}

 private:
  NodeType m_type;
};

}  // namespace linaro
#endif  // AST_H
