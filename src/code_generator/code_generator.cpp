#include "code_generator.h"

namespace Linaro {

#ifdef DEBUG
std::vector<Function*> CodeGenerator::m_functions;
#endif

std::unique_ptr<Function> CodeGenerator::compile(FunctionLiteral* AST) {
  CHECK(AST != nullptr);
  auto top_level = std::make_unique<Function>(AST, AST->name(), AST->numArgs());
  CodeGenerator cg;
  cg.m_fn = top_level.get();

#ifdef DEBUG
  m_functions.push_back(top_level.get());
#endif

  cg.compileFunction(&cg, AST);
  top_level->setIsCompiled(true);
  cg.generateBytecode(Bytecode::halt);
  return top_level;
}

void CodeGenerator::compileFunction(CodeGenerator* cg, FunctionLiteral* fn) {
  CHECK(cg != nullptr && fn != nullptr);
  // Create function scope (no param)
  cg->m_current_scope = std::make_unique<Scope>();

  // Define parameters in function scope
  for (const auto& arg : fn->args()) {
    cg->declareVariable(arg.name(), arg.loc());
  }

  fn->block()->visit(*cg);
}

void CodeGenerator::semanticError(const Location& loc, const char* format,
                                  ...) const {
  va_list args;
  va_start(args, format);
  Error::reportErrorAt(loc, Error::SemanticError, format, args);
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

void CodeGenerator::pushScope() {
  CHECK(m_current_scope != nullptr);
  // If outer scope is nullptr it's the top scope in the function which has
  // already been initialized.
  if (m_current_scope->outerScope() != nullptr)
    m_current_scope = std::make_unique<Scope>(m_current_scope);
}

void CodeGenerator::popScope() {
  CHECK(m_current_scope != nullptr);
  std::unique_ptr<Scope>& outer = m_current_scope->outerScope();
  if (outer != nullptr) {
    // Local scope being popped
    outer->addToIndex(m_current_scope->numLocals());
    m_current_scope = std::move(outer);
  } else {
    // Function scope being popped
    m_fn->setNumLocals(m_current_scope->index());
  }
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

int CodeGenerator::declareVariable(const std::string_view& name,
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

const Variable* CodeGenerator::resolveVariable(const std::string_view& name,
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
  const Variable* var =
      m_enclosing_compiler->resolveVariable(name, loc, is_assign);
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
  auto fn_literal = const_cast<FunctionLiteral*>(&node);
  auto fn = std::make_shared<Function>(fn_literal, node.name(), node.numArgs());

  // TODO: For lazy compilation it shouldn't compile here.
  CodeGenerator c(this);
  c.m_fn = fn.get();

#ifdef DEBUG
  m_functions.push_back(fn.get());
#endif

  compileFunction(&c, fn_literal);
  fn->setIsCompiled(true);

  // Create closure
  generateBytecode(Bytecode::closure, m_fn->addConstant(Value(fn)));

  // If it was a named function, it will have been forward declared in the
  // current scope, look it up and get the index. The closure is then stored at
  // this index in the local space at runtime.
  if (node.isNamed()) {
    int i = m_current_scope->resolveSymbol(node.name());
    CHECK(i != -1);
    generateBytecode(
        fn_literal->isTopLevel() ? Bytecode::gstore : Bytecode::store, i);
  }
  // Return null implicitly
  c.generateBytecode(Bytecode::null);
  c.generateBytecode(Bytecode::ret);
}

void CodeGenerator::visitArrayLiteral(const ArrayLiteral& node) {
  const auto& elements = node.elements();
  for (int i = node.size() - 1; i >= 0; i--) {
    elements[i]->visit(*this);
  }
  generateBytecode(Bytecode::new_array, node.elements().size());
}

void CodeGenerator::visitArrayAccess(const ArrayAccess& node) {
  node.target()->visit(*this);
  node.index()->visit(*this);
  generateBytecode(Bytecode::aload);
}

void CodeGenerator::visitIdentifier(const Identifier& node) {
  const Variable* var = resolveVariable(node.name(), node.loc(), false);
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
  generateBytecode(op, var->index());
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
    const Variable* var = resolveVariable(id->name(), id->loc(), true);
    int index;
    Bytecode op;
    if (var == nullptr) {
      // Variable was not defined, so define it at use:
      index = m_current_scope->defineSymbol(id->name());
      CHECK(index != -1);
      op = (m_enclosing_compiler == nullptr ? Bytecode::gstore
                                            : Bytecode::store);
    } else {
      // Variable was defined, reuse it:
      index = var->index();
      switch (var->origin()) {
        case VariableOrigin::top_level:
          op = Bytecode::gstore;
          break;
        case VariableOrigin::captured:
          op = Bytecode::cstore;
          break;
        case VariableOrigin::local:
          op = Bytecode::store;
          break;
      }
    }
    generateBytecode(op, index);
  } else if (target->isArrayAccess()) {
    ArrayAccess* ac = target->asArrayAccess();
    ac->target()->visit(*this);
    ac->index()->visit(*this);
    generateBytecode(Bytecode::astore);
  } else {
    semanticError(loc, "Left hand side of assignment invalid");
    return;
  }
}

void CodeGenerator::visitCall(const Call& node) {
  // Visit arguments and put them on stack before call.
  const auto& args = node.arguments();
  // Consider visiting them in the reverse order?
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
  pushScope();
  for (const auto& s : blk.getDeclarations()) {
    s->visit(*this);
  }

  for (const auto& s : blk.getStatements()) {
    s->visit(*this);
  }
  popScope();
}

void CodeGenerator::visitExpressionStatement(const ExpressionStatement& stmt) {
  stmt.expr()->visit(*this);
}

void CodeGenerator::visitFunctionDeclaration(const FunctionDeclaration& node) {
  declareVariable(node.symbol().asString(), node.loc());
}

void CodeGenerator::visitReturnStatement(const ReturnStatement& node) {
  visitExpressionForValue(node.expr());
  generateBytecode(Bytecode::ret);
}

void CodeGenerator::visitPrintStatement(const PrintStatement& node) {
  visitExpressionForValue(node.expr());
  generateBytecode(Bytecode::print);
}

void CodeGenerator::visitIfStatement(const IfStatement& node) {
  if (node.expr()->toBooleanIsTrue()) {
    std::cout << "RAHCED1" << std::endl;
    node.ifBlock()->visit(*this);
  } else if (node.expr()->toBooleanIsFalse()) {
    if (node.hasElseBlock()) {
      std::cout << "RAHCED2" << std::endl;
      node.elseBlock()->visit(*this);
    }
  } else {
    std::cout << "RAHCED3" << std::endl;
    // Visit condition.
    node.expr()->visit(*this);
    Label else_label(code()->currentOffset());
    // jump to jump at this point for and/or expr. Is fixed in the VM.
    generateBytecode(Bytecode::jmp_false, 0);
    node.ifBlock()->visit(*this);
    if (node.hasElseBlock()) {
      Label end_label(code()->currentOffset());
      generateBytecode(Bytecode::jmp, 0);
      code()->patchJump(else_label);
      // remove the evaluated expr since it's not used for its value.
      generateBytecode(Bytecode::pop);
      node.elseBlock()->visit(*this);
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
  node.whileBlock()->visit(*this);
  generateBytecode(Bytecode::jmp, start_of_block);
  code()->patchJump(end);
  generateBytecode(Bytecode::pop);
}

}  // namespace Linaro