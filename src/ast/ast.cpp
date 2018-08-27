#include "ast.h"
#include "expression.h"
#include "statement.h"

namespace linaro {

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

bool Expression::toBooleanIsFalse() { return !toBooleanIsTrue(); }

void FunctionLiteral::printNode() const {
  std::cout << "FunctionLiteral(";
  for (const auto& a : m_args) {
    a.printNode();
  }
  m_function_block->printNode();
  std::cout << ")" << std::endl;
}

// This needs some fixing probably
bool BinaryOperation::isValidReferenceExpression() {
  if (m_op.type() != Token::PERIOD) return false;
  return (m_left->isValidReferenceExpression() || m_left->isCall() ||
          m_right->isIdentifier());
}

}  // namespace linaro
