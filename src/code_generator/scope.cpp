#include "scope.h"
#include <iostream>
namespace Linaro {

int Scope::defineSymbol(const std::string_view& name) {
  auto it = m_symbol_table.find(name);
  if (it == m_symbol_table.end()) {
    m_symbol_table[name] = m_index;
    m_num_locals++;
    return m_index++;
  }

  // symbol already exist
  return -1;
}

int Scope::resolveSymbol(const std::string_view& name) {
  auto it = m_symbol_table.find(name);
  if (it != m_symbol_table.end()) {
    return m_symbol_table[name];
  }

  if (m_outer_scope != nullptr) {
    return m_outer_scope->resolveSymbol(name);
  }

  // variable doesn't exist in this function
  return -1;
}

}  // namespace Linaro