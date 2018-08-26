#ifndef OBJECT_H
#define OBJECT_H

#include <memory>

#include "../compiler/chunk.h"

namespace linaro {

class Value;
struct CapturedVariable;

#define OBJECTS(O) O(String) O(Function) O(Array) O(Closure) O(Thread)

class Object {
 public:
#define O(type) n##type,
  enum ObjectType : uint8_t { OBJECTS(O) };
#undef O

  Object(ObjectType type) : m_type{type} {}
  virtual ~Object() {}

  // Helper methods
#define O(type) \
  inline bool is##type() const { return m_type == ObjectType::n##type; }
  OBJECTS(O)
#undef O

  /* Linaro Object interface */

  // Conversions/checking
  virtual bool canBeNumber() const { return false; };
  virtual double asNumber() const { return 0; };
  virtual bool asBoolean() const { return false; };
  virtual std::string asString() const { return "Undefined"; };
  virtual size_t hash() const = 0;

 private:
  ObjectType m_type;
};

class String : public Object {
 public:
  String(const std::string& str) : Object{nString}, m_str{str} {}
  bool canBeNumber() const override;
  double asNumber() const override;
  bool asBoolean() const override {
    return (canBeNumber() && asNumber() != 0) || m_str == "true";
  }
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

class Function : public Object {
 public:
  Function(std::string name, int depth, int args)
      : Object{nFunction}, m_name{name}, m_depth{depth}, num_args{args} {}
  std::string asString() const override { return m_source; }
  size_t hash() const override { return std::hash<std::string>{}(m_source); }

  void setLocals(int num) { num_locals = num; }
  void setCode(const BytecodeChunk& c) { m_code = c; }
  int depth() const { return m_depth; }
  int numLocals() const { return num_locals; }
  int numArgs() const { return num_args; }
  int numCapturedVariables() const { return num_captured_variables; }
  bool isCompiled() const { return is_compiled; }
  BytecodeChunk* code() { return &m_code; }

  inline std::vector<Value>& constants() { return m_constants; }
  inline Value& getConstant(int i) { return m_constants[i]; }
  inline int numConstants() const { return m_constants.size(); }
  inline int addConstant(const Value& v) {
    m_constants.push_back(v);
    return m_constants.size() - 1;
  }

  CompilerCapturedVariable* getCapturedVariable(int i) {
    return &m_captured_variables[i];
  }

  // Debugging function
  void disassembleChunk() const { m_code.disassembleChunk(); }
  void printConstants() const;

 private:
  const Function* enclosing_function;
  std::string m_name;
  std::string m_source;
  bool is_compiled;  // for lazy compilation
  int m_depth;
  int num_args;
  int num_locals;
  int num_captured_variables;
  BytecodeChunk m_code;

  // Constants used in this function
  std::vector<Value> m_constants;

  std::vector<CompilerCapturedVariable> m_captured_variables;
};

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

class Array : public Object {
 public:
  // Array(std::initializer_list<Value> list);
  Array() : Object{nArray} {}
  Array(std::vector<Value> values) : Object{nArray}, m_values{values} {}
  inline void insert(const Value& val) {
    m_values.insert(m_values.begin(), val);
  }
  inline void append(const Value& val) { m_values.push_back(val); }
  int size() const { return m_values.size(); }
  std::vector<Value> getArray() const { return m_values; }
  void setDelimiter(char c) { delimiter = c; }
  Value& get(int i) { return m_values[i]; }

  bool canBeNumber() const override { return true; }
  double asNumber() const override;
  bool asBoolean() const override;
  std::string asString() const override;
  size_t hash() const override;

 private:
  std::vector<Value> m_values;
  char delimiter = ' ';
};

//This class will basically be what VM is now
class Thread : public Object {
  Thread() : Object{nThread} {}
  bool canBeNumber() const override { return false; }
  double asNumber() const override { return 0; }
  bool asBoolean() const override { return false; }
  std::string asString() const override { return "placeholder"; }
  size_t hash() const override { return 0; }
};

}  // Namespace linaro

#endif  // OBJECT_H
