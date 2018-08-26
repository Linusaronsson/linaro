#ifndef SCOPE_H
#define SCOPE_H

#include <unordered_map>

namespace linaro {
class Scope;

class Symbol {
 public:
  Symbol() {}
  Symbol(Scope* scope, std::string name, int index)
      : m_scope{scope}, m_name{name}, m_index{index} {}

  Scope* scope() const { return m_scope; }
  std::string name() const { return m_name; }
  int index() const { return m_index; }

 private:
  // The scope that defined this variable
  Scope* m_scope;
  std::string m_name;
  int m_index;
};

enum class ScopeType { Local, Global, Enclosed };

class Scope {
 public:
  Scope(ScopeType type, Scope* outer_scope = nullptr)
      : m_type{type}, m_outer_scope{outer_scope} {
    if (outer_scope != nullptr) m_num_locals = outer_scope->numLocals();
  }
  Scope* outerScope() const { return m_outer_scope; }
  // Returns index of the symbol. -1 if not found.
  int defineSymbol(const Symbol& sym);
  Symbol* resolveSymbol(std::string name);
  int numLocals() const { return m_num_locals; }
  bool isLocalScope() const { return m_type == ScopeType::Local; }
  bool isGlobalScope() const { return m_type == ScopeType::Global; }

 private:
  ScopeType m_type;
  Scope* m_outer_scope;
  // 0 by default if it's not a nested scope. Nested scopes
  // inherits this from it's parent.
  int m_num_locals = 0;
  std::unordered_map<std::string, Symbol> symbol_table;
};

}  // Namespace linaro

#endif  // SCOPE_H