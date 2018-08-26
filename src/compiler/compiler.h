#ifndef COMPILER_H
#define COMPILER_H

#include <unordered_map>

#include "../linaro_utils/utils.h"
#include "../vm/object.h"
#include "../vm/value.h"
#include "scope.h"


namespace linaro {

class Compiler {
 public:
  Compiler(Compiler* enclosing_compiler = nullptr)
      : m_enclosing_compiler{enclosing_compiler} {}
  ~Compiler() {}

  // Compiles top-level code. Calls compileFunction for every
  // nested function it encounters. This creates a new child
  // compiler that deals with that function.
  // Because it's the top-level function, it will not exist in
  // some constant pool. The caller is therefor responsible for
  // the created function.
  std::unique_ptr<Function> compile(std::string source, const char* filename);

 private:
  // The function compiled will live in the constant pool of the
  // enclosing function, returns a pointer to that.
  // Consider: what param should be passed? AST or source of function.
  Function* compileFunction();

  // Error reporting during AST visit
  void semanticError(const Location& loc, const char* format, ...);

  // Emitting raw data to current chunk
  inline void emitByte(uint8_t byte) { code()->addByte(byte); }
  inline void emit16Bits(uint16_t op) { code()->add16Bits(op); }
  inline void emit32Bits(uint32_t op) { code()->add32Bits(op); }

  // Emitting bytecodes
  inline void generateBytecode(Bytecode op_code) { emitByte((uint8_t)op_code); }
  inline void generateBytecode(Bytecode op_code, uint16_t operand) {
    emitByte((uint8_t)op_code);
    emit16Bits(operand);
  }
  inline void generateBytecode(Bytecode op_code, uint16_t operand1,
                               uint16_t operand2) {
    generateBytecode(op_code, operand1);
    emit16Bits(operand2);
  }

  inline void generateConstant(const Value& val) {
    generateBytecode(Bytecode::constant, m_fn->addConstant(val));
  }

  // This is used for string and number constants, because they should uniquely
  // exist in the const pool.
  // Other values will always be unique by default anyway so generateConstants()
  // will be used for those.
  inline void generateConstantIfNew(const Value& val) {
    generateBytecode(Bytecode::constant, addConstantIfNew(val));
  }

  // Checks m_constants (hash map) if 'val' is in there,
  // if not, add it.
  int addConstantIfNew(const Value& val);

  // Symbols
  int defineSymbol(const Symbol& sym, const Location& loc);
  Symbol* resolveSymbol(std::string name) const;
  int numLocals() const { return current_scope->numLocals(); }
  // If the symbol was in an enclosing function, add it to the
  // list if captured variables in 'm_fn'.
  void addCapturedSymbol(int index, bool is_local);

  // Gets chunk of function being compiled.
  BytecodeChunk* code() { return m_fn->code(); }

  // Compiler for enclosing function. 'nullptr' if top-level.
  Compiler* m_enclosing_compiler;

  // Function being compiled. Added to constants of enclosing
  // functions.
  Function* m_fn;

  // Constant pool. Currently only used for numbers/strings.
  // Other constants are put directly in constants of m_fn.
  std::unordered_map<Value, int, Value::ValueHasher> constant_pool;

  // Current scope
  Scope* current_scope;

  // Currently active scopes. Current scope is the TOS.
  Stack<Scope> scopes;
};

}  // Namespace linaro

#endif  // COMPILER_H