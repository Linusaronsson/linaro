#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <memory>
#include <unordered_map>

#include "../code_generator/chunk.h"
#include "value.h"

namespace linaro {

class String : public Object {
 public:
  explicit String(std::string_view str) : Object{nString}, m_str{str} {}
  bool canBeNumber() const override;
  double asNumber() const override;
  bool asBoolean() const override { return !m_str.empty(); }
  std::string asString() const override { return m_str; }
  size_t hash() const override { return std::hash<std::string>{}(m_str); }

 private:
  const std::string m_str;
};

struct CompilerCapturedVariable {
  // Index into the local space that the captured variable exists.
  int index;
  // If it's a local one index points to the local variable space of it's
  // parent function (currently on the top of call stack). If it's not local,
  // it's a captured variable from an outer function, which has already been
  // captured by the enclosing function, so reuse it.
  bool is_local;
};

class FunctionLiteral;  // Function AST node
class Function : public Object {
 public:
  Function(FunctionLiteral* fn_ast, std::string_view name, int num_args)
      : Object{nFunction},
        m_fn_ast{fn_ast},
        m_name{name},
        m_num_args{num_args} {}
  std::string asString() const override { return std::string(m_name); }
  size_t hash() const override {
    return std::hash<std::string_view>{}(m_source);
  }

  void setNumLocals(int num) { m_num_locals = num; }
  void setCode(const BytecodeChunk& c) { m_code = c; }
  void setIsCompiled(bool is_compiled) { m_is_compiled = is_compiled; }
  std::string_view name() const { return m_name; }
  int numLocals() const { return m_num_locals; }
  int numArgs() const { return m_num_args; }
  int numCapturedVariables() const { return m_num_captured_variables; }
  bool isCompiled() const { return m_is_compiled; }
  BytecodeChunk* code() { return &m_code; }
  FunctionLiteral* getFunctionAST() const { return m_fn_ast; }

  inline std::vector<Value>& constants() { return m_constants; }
  inline Value& getConstant(int i) { return m_constants[i]; }
  inline int numConstants() const { return m_constants.size(); }
  inline int addConstant(const Value& v) {
    m_constants.push_back(v);
    return m_constants.size() - 1;
  }

  auto& getCapturedVariables() const { return m_captured_variables; }
  int addCapturedVariable(int index, bool is_local);
  CompilerCapturedVariable* getCapturedVariable(int i) {
    return &m_captured_variables[i];
  }

#ifdef DEBUG
  void printCapturedVariables() const;
  void printFunction();
  void disassembleChunk() const { m_code.disassembleChunk(); }
  void printConstants();
#endif

 private:
  FunctionLiteral* m_fn_ast;
  std::string_view m_name;
  std::string_view m_source;
  bool m_is_compiled = false;  // for lazy compilation
  int m_num_args;
  int m_num_locals;
  int m_num_captured_variables;
  BytecodeChunk m_code;

  // Constants used in this function
  std::vector<Value> m_constants;

  std::vector<CompilerCapturedVariable> m_captured_variables;
};

struct CapturedVariable;
class Closure : public Object {
 public:
  Closure(Function* fn) : Object(nClosure), m_fn{fn} {}
  std::string asString() const override { return m_fn->asString(); };
  size_t hash() const override { return m_fn->hash(); }

  Function* fun() const { return m_fn; }
  std::vector<CapturedVariable*>& getCapturedVariables() {
    return m_captured_variables;
  }

  void addCapturedVariable(CapturedVariable* cv) {
    m_captured_variables.push_back(cv);
  }

  CapturedVariable* getCapturedVariable(int x) {
    return m_captured_variables[x];
  }

 private:
  // The function this closure is an instance of
  Function* m_fn;

  // Variables captured from outer functions
  // The captured variables will exist in a linked list of all
  // captured variables in the VM. (later in one per thread).
  std::vector<CapturedVariable*> m_captured_variables;
};

// This class will basically be what VM is now
class Thread : public Object {
  Thread() : Object{nThread} {}
  bool canBeNumber() const override { return false; }
  double asNumber() const override { return 0; }
  bool asBoolean() const override { return false; }
  std::string asString() const override { return "placeholder"; }
  size_t hash() const override { return 0; }
};

class Array : public Object {
 public:
  // Array(std::initializer_list<Value> list);
  Array() : Object{nArray} {}

  inline Value& get(const Value& v) { return m_values[v]; }
  inline void insert(const Value& key, const Value& val) {
    m_values.insert({key, val});
  }

  int size() const { return m_values.size(); }
  const auto& getArray() const { return m_values; }
  void setDelimiter(char c) { delimiter = c; }

  bool canBeNumber() const override { return true; }
  double asNumber() const override;
  bool asBoolean() const override;
  std::string asString() const override;
  size_t hash() const override;

 private:
  // std::vector<Value> m_values;
  std::unordered_map<Value, Value, Value::ValueHasher> m_values;
  char delimiter = ' ';
};

}  // Namespace linaro

#endif  // OBJECT_H
