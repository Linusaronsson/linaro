#include "code_generator.h"

namespace linaro {

std::unique_ptr<Function> CodeGenerator::compile(FunctionLiteral* AST) {
  CHECK(AST != nullptr);
  auto top_level = std::make_unique<Function>(AST, AST->name(), AST->numArgs());
  top_level->getFunctionAST()->visit(*this);
  top_level->setIsCompiled(true);  // Probably unecessary
  generateBytecode(Bytecode::halt);
  return top_level;
}

void CodeGenerator::compileFunction(Function* fn) {
  CHECK(fn != nullptr);
  // Create a code generator for the function to be compiled
  CodeGenerator c{fn, m_enclosing_compiler};
  fn->getFunctionAST()->visit(c);
  fn->setIsCompiled(true);  // Probably unecessary
}

void CodeGenerator::semanticError(const Location& loc, const char* format,
                                  ...) const {
  va_list args;
  va_start(args, format);
  Error::reportErrorAt(loc, Error::RuntimeError, format, args);
  va_end(args);
}

void CodeGenerator::emitByte(uint8_t byte) { code()->addByte(byte); }
void CodeGenerator::emit16Bits(uint16_t op) { code()->add16Bits(op); }
void CodeGenerator::emit32Bits(uint32_t op) { code()->add32Bits(op); }

void CodeGenerator::generateBytecode(Bytecode op_code) {
  emitByte((uint8_t)op_code);
}
void CodeGenerator::generateBytecode(Bytecode op_code, uint16_t operand) {
  emitByte((uint8_t)op_code);
  emit16Bits(operand);
}

void CodeGenerator::generateBytecode(Bytecode op_code, uint16_t operand1,
                                     uint16_t operand2) {
  generateBytecode(op_code, operand1);
  emit16Bits(operand2);
}

void CodeGenerator::generateConstant(const Value& val) {
  generateBytecode(Bytecode::constant, m_fn->addConstant(val));
}

int CodeGenerator::addConstantIfNew(const Value& val) {
  auto it = m_constant_pool.find(val);
  if (it == m_constant_pool.end()) {
    // Add it to function's constants, which returns the index it was
    // inserted at.
    int index = m_fn->addConstant(val);
    // Add it to constant pool for later checking, remember the index.
    m_constant_pool.insert({val, index});
    return index;
  }
  // Constant was already in constant pool, return it's index.
  return it->second;
}

void CodeGenerator::generateConstantIfNew(const Value& val) {
  generateBytecode(Bytecode::constant, addConstantIfNew(val));
}

int CodeGenerator::defineSymbol(const std::string_view& name,
                                const Location& loc) {
  int res = m_current_scope->defineSymbol(name);
  if (res == -1) {
    semanticError(loc, "Identifier already taken: '%s'",
                  std::string(name).c_str());
  }
  return 0;
}

const Variable* CodeGenerator::addVariable(int index, VariableOrigin origin) {
  return &(*m_variables.insert({index, origin}).first);
}

const Variable* CodeGenerator::resolveSymbol(const std::string_view& name,
                                             const Location& loc,
                                             bool is_assign) {
  int arg = m_current_scope->resolveSymbol(name);
  if (arg != -1)
    return addVariable(arg, m_enclosing_compiler == nullptr
                                ? VariableOrigin::top_level
                                : VariableOrigin::local);

  // Symbol was not found anywhere in current function, check the enclosing
  // function:

  // No enclosing function so the symbol was not found anywhere.
  if (m_enclosing_compiler == nullptr) {
    // Only print error if it was NOT an assignment. An assignment defines the
    // symbol if it didn't already exist.
    if (!is_assign)
      semanticError(loc, "Identifier not defined '%s'",
                    std::string(name).c_str());
    return nullptr;
  }

  // Check enclosing function
  const Variable* var = m_enclosing_compiler->resolveSymbol(name, loc, false);
  // If var is nullptr we can just return it because the enclosing compiler has
  // already reported that the symbol wasn't defined.
  if (var == nullptr || var->is_top_level())
    return var;
  else if (var->is_local()) {
    arg = m_fn->addCapturedVariable(var->index(), true);  // true means local
  } else if (var->is_captured()) {
    arg = m_fn->addCapturedVariable(var->index(), false);
  }
  return addVariable(arg, VariableOrigin::captured);
}

void CodeGenerator::pushScope() {
  m_current_scope = std::make_unique<Scope>(m_current_scope);
}

void CodeGenerator::popScope() {
  std::unique_ptr<Scope>& outer = m_current_scope->outerScope();
  outer->addToIndex(m_current_scope->numLocals());
  m_current_scope = std::move(outer);
}

/* --- Visit Expressions --- */

void CodeGenerator::visitNullExpression(const NullExpression& ne) {
  // If this is reached, an error has occured, so just return.
  return;
}

void CodeGenerator::visitLiteral(const Literal& val) {
  Value v = val.value();
  if (v.isString() || v.isNumber()) {
    generateConstantIfNew(v);
  } else if (v.isBoolean()) {
    Bytecode b = v.asBoolean() ? Bytecode::TRUE : Bytecode::FALSE;
    generateBytecode(b);
  } else if (v.isNoll()) {
    // seems uncessary to store a NULL value in the null token?
    generateBytecode(Bytecode::null);
  }
}

void CodeGenerator::visitFunctionLiteral(const FunctionLiteral& node) {
  // Function fn{const_cast<FunctionLiteral*>(&node), node.name(),
  // node.numArgs()}; int index = m_fn->addConstant(Value(fn));
  // Variable* var = resolveSymbol()

  // Create closure
  // generateBytecode(Bytecode::closure, index);
}

void CodeGenerator::visitArrayLiteral(const ArrayLiteral& node) {}

void CodeGenerator::visitArrayAccess(const ArrayAccess& node) {}

void CodeGenerator::visitIdentifier(const Identifier& node) {
  const Variable* var = resolveSymbol(node.name(), node.loc(), false);
  if (var == nullptr) return;
  Bytecode op;
  switch (var->origin()) {
    case VariableOrigin::top_level:
      op = Bytecode::gload;
      break;
    case VariableOrigin::captured:
      op = Bytecode::cload;
      break;
    case VariableOrigin::local:
      op = Bytecode::load;
      break;
  }
  generateBytecode(op);
}

void CodeGenerator::visitBinaryOperation(const BinaryOperation& node) {
  TokenType op = node.op().type();
  if (Token::isComparisonOp(op)) {
    visitComparisonExpression(node);
  } else {
    switch (op) {
      case TokenType::OR:
        visitOrExpression(node);
        break;
      case TokenType::AND:
        visitAndExpression(node);
        break;
      default:
        visitArithmeticExpression(node);
    }
  }
}

void CodeGenerator::visitOrExpression(const BinaryOperation& node) {
  auto left = node.leftOperand();
  auto right = node.rightOperand();
  if (left->toBooleanIsTrue()) {
    left->visit(*this);
  } else if (left->toBooleanIsFalse()) {
    right->visit(*this);
  } else {
    left->visit(*this);
    Label lbl(code()->currentOffset());
    generateBytecode(Bytecode::jmp_true, 0);
    right->visit(*this);
    code()->patchJump(lbl);
  }
}

void CodeGenerator::visitAndExpression(const BinaryOperation& node) {
  auto left = node.leftOperand();
  auto right = node.rightOperand();
  if (left->toBooleanIsFalse()) {
    left->visit(*this);
  } else if (left->toBooleanIsTrue()) {
    right->visit(*this);
  } else {
    left->visit(*this);
    Label lbl(code()->currentOffset());
    generateBytecode(Bytecode::jmp_false, 0);
    right->visit(*this);
    code()->patchJump(lbl);
  }
}

void CodeGenerator::visitComparisonExpression(const BinaryOperation& node) {
  // todo: interpret x > y > z as x > y && y > z
  node.leftOperand()->visit(*this);
  node.rightOperand()->visit(*this);
  switch (node.op().type()) {
    case TokenType::EQ:
      generateBytecode(Bytecode::eq);
      break;
    case TokenType::NE:
      generateBytecode(Bytecode::neq);
      break;
    case TokenType::LT:
      generateBytecode(Bytecode::lt);
      break;
    case TokenType::LTE:
      generateBytecode(Bytecode::lte);
      break;
    case TokenType::GT:
      generateBytecode(Bytecode::gt);
      break;
    case TokenType::GTE:
      generateBytecode(Bytecode::gte);
      break;
    default:
      UNREACHABLE();
  }
}

void CodeGenerator::visitArithmeticExpression(const BinaryOperation& node) {
  node.leftOperand()->visit(*this);
  node.rightOperand()->visit(*this);
  switch (node.op().type()) {
    case TokenType::ADD:
      generateBytecode(Bytecode::add);
      break;
    case TokenType::SUB:
      generateBytecode(Bytecode::sub);
      break;
    case TokenType::MUL:
      generateBytecode(Bytecode::mul);
      break;
    case TokenType::DIV:
      generateBytecode(Bytecode::div);
      break;
    case TokenType::MOD:
      generateBytecode(Bytecode::mod);
      break;
    case TokenType::EXP:
      generateBytecode(Bytecode::exp);
      break;
    default:
      UNREACHABLE();
  }
}

void CodeGenerator::visitUnaryOperation(const UnaryOperation& node) {
  auto operand = node.operand();
  auto op = node.op();
  operand->visit(*this);
  switch (op.type()) {
    case TokenType::SUB:
      generateBytecode(Bytecode::neg);
      break;
    case TokenType::NOT:
      generateBytecode(Bytecode::NOT);
    case TokenType::INCR:
    case TokenType::DECR: {
      Bytecode inc_or_dec =
          op.type() == TokenType::INCR ? Bytecode::incr : Bytecode::decr;
      if (node.isPrefix()) {
        generateBytecode(inc_or_dec);
        generateBytecode(Bytecode::dup);
        visitAssignmentTarget(operand, op.getLocation());
      } else {
        generateBytecode(Bytecode::dup);
        generateBytecode(inc_or_dec);
        visitAssignmentTarget(operand, op.getLocation());
      }
      break;
    }
    default:
      UNREACHABLE();
  }
}

void CodeGenerator::visitExpressionForValue(Expression* expr) {
  expr->visit(*this);

  // fix this (implement a parse node for LogicalOperation).
  // REMOVE MAYBE?
  if (expr->isBinaryOperation() &&
      expr->asBinaryOperation()->isLogicalOperation()) {
    generateBytecode(Bytecode::to_bool);
  }
}

void CodeGenerator::visitAssignment(const Assignment& node) {
  visitExpressionForValue(node.rightOperand());
  // Expect the correct value to now be on top of operand stack.
  // store it in given variable:
  visitAssignmentTarget(node.target(), node.loc());
}

void CodeGenerator::visitAssignmentTarget(Expression* target,
                                          const Location& loc) {
  if (target->isIdentifier()) {
    Identifier* id = target->asIdentifier();
    const Variable* var = resolveSymbol(id->name(), id->loc(), true);
    int index;
    Bytecode op;
    if (var == nullptr) {
      // Variable was not defined, so define it at use:
      index = m_current_scope->defineSymbol(id->name());
      CHECK(index != -1);
      op = (m_enclosing_compiler == nullptr ? Bytecode::gload : Bytecode::load);
    } else {
      // Variable was defined, reuse it:
      index = var->index();
      switch (var->origin()) {
        case VariableOrigin::top_level:
          op = Bytecode::gload;
          break;
        case VariableOrigin::captured:
          op = Bytecode::cload;
          break;
        case VariableOrigin::local:
          op = Bytecode::load;
          break;
      }
    }
    generateBytecode(op, index);
  } else {
    semanticError(loc, "Left hand side of assignment invalid");
    return;
  }
}  // namespace linaro

void CodeGenerator::visitCall(const Call& node) {
  // Visit arguments and put them on stack before call.
  const auto& args = node.arguments();
  int arity = args.size();
  for (int i = args.size() - 1; i >= 0; i--) {
    args[i]->visit(*this);
  }
  node.caller()->visit(*this);
  generateBytecode(call_tos, arity);
}

/* ---  statements --- */
void CodeGenerator::visitBlock(const Block& blk) {
  // Visit declarations first for forward references (only functions atm).
  for (const auto& s : blk.getDeclarations()) {
    s->visit(*this);
  }

  for (const auto& s : blk.getStatements()) {
    s->visit(*this);
  }
}

void CodeGenerator::visitExpressionStatement(const ExpressionStatement& stmt) {
  stmt.expr()->visit(*this);
}

void CodeGenerator::visitFunctionDeclaration(const FunctionDeclaration& node) {
  defineSymbol(node.symbol().asString(), node.loc());
}

void CodeGenerator::visitReturnStatement(const ReturnStatement& node) {
  visitExpressionForValue(node.expr());
  generateBytecode(Bytecode::ret);
}

void CodeGenerator::visitPrintStatement(const PrintStatement& node) {
  visitExpressionForValue(node.expr());
  generateBytecode(Bytecode::print);
}

void CodeGenerator::visitLocalScope(Block* blk) {
  pushScope();
  blk->visit(*this);
  popScope();
}

void CodeGenerator::visitIfStatement(const IfStatement& node) {
  if (node.expr()->toBooleanIsTrue()) {
    visitLocalScope(node.ifBlock());
  } else if (node.expr()->toBooleanIsFalse()) {
    if (node.hasElseBlock()) {
      visitLocalScope(node.elseBlock());
    }
  } else {
    // Visit condition.
    node.expr()->visit(*this);
    Label else_label(code()->currentOffset());
    // jump to jump at this point for and/or expr. Is fixed in the VM.
    generateBytecode(Bytecode::jmp_false, 0);
    visitLocalScope(node.ifBlock());
    if (node.hasElseBlock()) {
      Label end_label(code()->currentOffset());
      generateBytecode(Bytecode::jmp, 0);
      code()->patchJump(else_label);
      // remove the evaluated expr since it's not used for its value.
      generateBytecode(Bytecode::pop);
      visitLocalScope(node.elseBlock());
      code()->patchJump(end_label);
    } else {
      code()->patchJump(else_label);
      generateBytecode(Bytecode::pop);
    }
  }
}

void CodeGenerator::visitWhileStatement(const WhileStatement& node) {
  int start_of_block = code()->currentOffset() - 1;
  node.expr()->visit(*this);
  Label end(code()->currentOffset());
  generateBytecode(Bytecode::jmp_false, 0);
  visitLocalScope(node.whileBlock());
  generateBytecode(Bytecode::jmp, start_of_block);
  code()->patchJump(end);
  generateBytecode(Bytecode::pop);
}

}  // Namespace linaro