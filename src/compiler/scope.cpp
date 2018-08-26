#include "scope.h"

namespace linaro {

int Scope::defineSymbol(const Symbol& sym) {
  std::string name = sym.name();
  auto it = symbol_table.find(name);
  if (it == symbol_table.end()) {
    symbol_table[name] = sym;
    m_num_locals++;
    return 0;
  }

  // symbol already exist
  return -1;
}

Symbol* Scope::resolveSymbol(std::string name) {
  auto it = symbol_table.find(name);
  if (it != symbol_table.end()) {
    return &symbol_table[name];
  }

  if (m_outer_scope != nullptr) {
    return m_outer_scope->resolveSymbol(name);
  }

  // variable doesn't exist in this function
  return nullptr;
}

}  // namespace linaro