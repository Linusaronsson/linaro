#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>

namespace linaro {
class Symbol {
 public:
  Symbol(std::string name, int index) : m_name{name}, m_index{index} {}
  std::string name() const { return m_name; }
  int index() const { return m_index; }

 private:
  std::string m_name;
  // Index into corresponding variable space at runtime. Either
  // a local space or for a captured variable.
  int m_index;
};

}  // Namespace linaro

#endif  // SYMBOL_TABLE_H