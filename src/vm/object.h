#ifndef OBJECT_H
#define OBJECT_H

#include <memory>

#include "../compiler/chunk.h"

namespace linaro {

class ConstantPool;
class Value;

class Object {
 public:
  virtual ~Object() {}

  /* Linaro Object interface */

  // Conversions/checking
  virtual bool canBeNumber() const { return false; };
  virtual double asNumber() const { return 0; };
  virtual bool asBoolean() const { return false; };
  virtual std::string asString() const { return "Undefined"; };

  // For c++ hash maps
  virtual size_t hash() const = 0;
  //  virtual bool operator==(const Object& lhs) = 0;
};

class String : public Object {
 public:
  String(const std::string& str) : m_str{str} {}
  bool canBeNumber() const override;
  double asNumber() const override;
  bool asBoolean() const override {
    return (canBeNumber() && asNumber() != 0) || m_str == "true";
  }
  std::string asString() const override { return m_str; }
  size_t hash() const override { return std::hash<std::string>{}(m_str); }

 private:
  std::string m_str;
};

class Function : public Object {
 public:
  Function(std::string name, int depth, int args)
      : m_name{name}, m_depth{depth}, num_args{args} {}
  std::string asString() const override { return m_source; }
  size_t hash() const override { return std::hash<std::string>{}(m_source); }

  void setLocals(int num) { num_locals = num; }
  void SetCodeChunk(const BytecodeChunk& c) { m_code = c; }
  int depth() const { return m_depth; }
  int NumLocals() const { return num_locals; }
  int NumArgs() const { return num_args; }
  BytecodeChunk* Code() { return &m_code; }

 private:
  const Function* enclosing_function;
  std::string m_name;
  std::string m_source;
  bool is_compiled;  // for lazy compilation
  int m_depth;
  int num_args;
  int num_locals;
  BytecodeChunk m_code;
  // All the constants in the function
  std::shared_ptr<ConstantPool> m_constants;
};

class Array : public Object {
 public:
  // Array(std::initializer_list<Value> list);
  Array() {}
  Array(std::vector<Value> values) : m_values{values} {}
  void insert(const Value& val) { m_values.insert(m_values.begin(), val); }
  void append(const Value& val) { m_values.push_back(val); }
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

class Thread : public Object {
  bool canBeNumber() const override { return false; }
  double asNumber() const override { return 0; }
  bool asBoolean() const override { return false; }
  std::string asString() const override { return "placeholder"; }
  size_t hash() const override { return 0; }
};

}  // Namespace linaro

#endif  // OBJECT_H
