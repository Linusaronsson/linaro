#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include <unordered_map>
#include <vector>

#include "value.h"

namespace linaro {

/*
 * A constant pool stores frequently used runtime Values.
 * TODO: Implement Value first (define print/hash methods) because
 * this file depends on it.
 */
class ConstantPool {
 private:
  int m_index = 0;
  // TODO: change to hash map (needed for addIfNew)
  // std::vector<Value> m_constants;
  std::unordered_map<int, Value*> m_constants;

 public:
  //   int addIfNew(const Value& val);
  //   int addAlways(const Value& val);
  //   void printConstantPool();
  //
  //   // Retrieving a value
  //   Value& operator[](int n) { return *m_constants[n]; }
  //   Value& getConstant(int index) { return *m_constants[index]; }
  //
  //   int size() const { return m_constants.size(); }
  //   const auto& constants() { return m_constants; }
  //   int index() { return m_index; }
  //   void addConstant(Value& v) { /* m_constants[m_index].insert(m_index++,
  //   v); */
};  // namespace linaro

}  // Namespace linaro

#endif  // CONSTANT_POOL_H
