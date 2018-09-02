#include "ast.h"
#include "expression.h"
#include "statement.h"

namespace Linaro {

#define T(type) \
  type* Node::as##type() { return static_cast<type*>(this); }
AST_NODES(T)
#undef T

bool Expression::isFalse() { return isLiteral() && asLiteral()->isFalse(); }
bool Expression::isTrue() { return isLiteral() && asLiteral()->isTrue(); }
bool Expression::isNullLiteral() {
  return isLiteral() && asLiteral()->value().isNoll();
}

bool Expression::isNumberLiteral() {
  return isLiteral() && asLiteral()->value().isNumber();
}

bool Expression::isStringLiteral() {
  return isLiteral() && asLiteral()->value().isString();
}

bool Expression::toBooleanIsTrue() {
  return isLiteral() && asLiteral()->value().asBoolean();
}

bool Expression::toBooleanIsFalse() {
  return isLiteral() && !asLiteral()->value().asBoolean();
}

void FunctionLiteral::printNode() const {
  std::cout << "FunctionLiteral: {\n";
  std::cout << "Name: " << m_function_name << '\n';
  std::cout << "Arguments: ";
  for (auto it = m_args.begin(); it != m_args.end(); it++) {
    it->printNode();
    if (std::next(it) != m_args.end()) std::cout << ", ";
  }
  std::cout << "\n\n";
  m_function_block->printNode();
  std::cout << "\n";
  std::cout << "} // End of FunctionLiteral: " << m_function_name << "\n";
}

// This needs some fixing probably
bool BinaryOperation::isValidReferenceExpression() {
  if (m_op.type() != TokenType::PERIOD) return false;
  return (m_left->isValidReferenceExpression() || m_left->isCall() ||
          m_right->isIdentifier());
}

}  // namespace Linaro
