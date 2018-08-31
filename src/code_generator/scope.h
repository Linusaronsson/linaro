#ifndef SCOPE_H
#define SCOPE_H

#include <memory>
#include <unordered_map>

namespace linaro {

class Scope {
 public:
  Scope() : m_index{0} {}
  Scope(std::unique_ptr<Scope>& outer_scope)
      : m_outer_scope{std::move(outer_scope)}, m_index{outer_scope->m_index} {}

  std::unique_ptr<Scope>& outerScope() { return m_outer_scope; }
  // Returns index of the symbol. -1 if not found.
  int defineSymbol(const std::string_view& name);
  int resolveSymbol(const std::string_view& name);
  int numLocals() const { return m_num_locals; }
  int index() const { return m_index; }
  void addToIndex(int n) { m_index += n; }

 private:
  std::unique_ptr<Scope> m_outer_scope;
  // Current index, inherited from parent scope's 'm_index'.
  int m_index;
  // Number of locals. Is added to enclosing scope's 'm_index' when this scope
  // is exited.
  int m_num_locals = 0;
  std::unordered_map<std::string_view, int> m_symbol_table;
};

}  // Namespace linaro

#endif  // SCOPE_H