#include "compiler.h"

namespace linaro {

int Compiler::addConstantIfNew(const Value& val) {
  auto it = constant_pool.find(val);
  if (it == constant_pool.end()) {
    // Add it to function's constants, which returns the index it was
    // inserted at.
    int index = m_fn->addConstant(val);
    // Add it to constant pool for later checking, remember the index.
    constant_pool.insert({val, index});
    return index;
  }
  // Constant was already in constant pool, return it's index.
  return it->second;
}

void Compiler::semanticError(const Location& loc, const char* format, ...) {
  va_list args;
  va_start(args, format);
  Error::reportErrorAt(loc, Error::RuntimeError, format, args);
  va_end(args);
}

int Compiler::defineSymbol(const Symbol& sym, const Location& loc) {
  int res = current_scope->defineSymbol(sym);
  if (res == -1) {
    semanticError(loc, "Identifier already taken: '%s'", sym.name().c_str());
  }
  return 0;
}
Symbol* Compiler::resolveSymbol(std::string name) const {
  Symbol* arg = current_scope->resolveSymbol(name);

    // Symbol didn't exist in current function, check enclosing function.
    if (arg == nullptr) {
      Symbol* arg = m_enclosing_compiler->resolveSymbol(name);
      return nullptr;
    }
  return arg;
}
void addCapturedSymbol(int index, bool is_local);

}  // Namespace linaro