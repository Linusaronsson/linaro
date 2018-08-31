#ifndef COMPILER_H
#define COMPILER_H

#include <unordered_map>
#include <unordered_set>

#include "../ast/ast.h"
#include "../linaro_utils/utils.h"
#include "../parsing/parser.h"
#include "../vm/object.h"
#include "../vm/value.h"
#include "chunk.h"
#include "scope.h"

namespace linaro {

// Keeps track of where a symbol was resolved from in relation to the current
// function.
enum class VariableOrigin : uint8_t { top_level, captured, local };

struct Variable {
 public:
  Variable(int index, VariableOrigin origin)
      : m_index{index}, m_origin{origin} {}
  int index() const { return m_index; }
  bool is_top_level() const { return m_origin == VariableOrigin::top_level; }
  bool is_captured() const { return m_origin == VariableOrigin::captured; }
  bool is_local() const { return m_origin == VariableOrigin::local; }
  VariableOrigin origin() const { return m_origin; }

  virtual bool operator==(const Variable& lhs) const {
    return m_index == lhs.m_index && m_origin == lhs.m_origin;
  }

  struct VariableHasher {
    size_t operator()(const Variable& v) const noexcept {
      return v.m_index * 31 + (int)v.m_origin;
    }
  };

 private:
  int m_index;
  VariableOrigin m_origin;
};

class CodeGenerator : public NodeVisitor {
 public:
  CodeGenerator(Function* fn, CodeGenerator* enclosing_compiler = nullptr)
      : m_fn{fn}, m_enclosing_compiler{enclosing_compiler} {}
  ~CodeGenerator() {}

  // Compiles top-level code. Calls compileFunction for every
  // nested function it encounters. This creates a new child
  // compiler that deals with that function.
  // Because it's the top-level function, it will not exist in
  // some constant pool. The caller is therefor responsible for
  // the created function.
  std::unique_ptr<Function> compile(FunctionLiteral* AST);

 private:
  // During visitation of a FunctionLiteral, a Function object is added to
  // parent constant pool (which stores the AST of the function). When the
  // function is later called, it will be looked up in the const pool to get a
  // pointer to it. Then this function is called by passing it that pointer in
  // order to compile it.
  void compileFunction(Function* fn);

  // Error reporting during AST visit
  void semanticError(const Location& loc, const char* format, ...) const;

  // Gets chunk of function being compiled.
  inline BytecodeChunk* code() { return m_fn->code(); }

  // Emitting raw data to current chunk
  inline void emitByte(uint8_t byte);
  inline void emit16Bits(uint16_t op);
  inline void emit32Bits(uint32_t op);

  // Emitting bytecodes
  inline void generateBytecode(Bytecode op_code);
  inline void generateBytecode(Bytecode op_code, uint16_t operand);
  inline void generateBytecode(Bytecode op_code, uint16_t operand1,
                               uint16_t operand2);

  // Generating constants for the constant pool
  inline void generateConstant(const Value& val);

  // Checks m_constants (hash map) if 'val' is in there,
  // if not, add it.
  int addConstantIfNew(const Value& val);

  // This is used for string and number constants, because they should uniquely
  // exist in the const pool.
  // Other values will always be unique by default anyway so generateConstants()
  // will be used for those.
  inline void generateConstantIfNew(const Value& val);

  // Symbols
  int defineSymbol(const std::string_view& name, const Location& loc);
  inline const Variable* addVariable(int index, VariableOrigin origin);
  const Variable* resolveSymbol(const std::string_view& name,
                                const Location& loc, bool is_assign);
  int numLocals() const { return m_current_scope->numLocals(); }

  // Scopes
  inline void pushScope();
  void popScope();

  // Visitation methods for all nodes in the AST for generating bytecode.
#define T(type) void visit##type(const type& node) override;
  AST_NODES(T)
#undef T

  void visitExpressionForValue(Expression* expr);
  void visitLocalScope(Block* blk);
  void visitAssignmentTarget(Expression* target, const Location& loc);
  // Visitation methods custom for this class:

  // Visit binary op will dispatch to one of these:
  void visitOrExpression(const BinaryOperation& op);
  void visitAndExpression(const BinaryOperation& op);
  void visitArithmeticExpression(const BinaryOperation& op);
  void visitComparisonExpression(const BinaryOperation& op);

  // Function being compiled. Added to constants of enclosing
  // functions.
  Function* m_fn;

  // CodeGenerator for enclosing function. 'nullptr' if top-level.
  CodeGenerator* m_enclosing_compiler;

  // Constant pool. Currently only used for numbers/strings.
  // Other constants are put directly in constants of m_fn.
  std::unordered_map<Value, int, Value::ValueHasher> m_constant_pool;

  // Linked-list of all scopes
  std::unique_ptr<Scope> m_current_scope;

  // Hash set of variables
  std::unordered_set<Variable, Variable::VariableHasher> m_variables;
};

}  // Namespace linaro

#endif  // COMPILER_H