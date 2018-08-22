#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "value.h"

namespace linaro {
/*
 * A constant pool stores frequently used runtime Values.
 * TODO: Implement Value first (define print/hash methods) because
 * this file depends on it.
 */
class ConstantPool {
 public:
  int addIfNew(const Value& val);
  int addAlways(const Value& val);
  void printConstantPool();

  // Retrieving a value
  Value& operator[](int n) { return *m_constants[n]; }
  Value& getConstant(int index) { return (*this)[index]; }

  size_t size() const { return m_constants.size(); }
  auto constants() { return m_constants; }
  int index() { return m_index; }

 private:
  std::unordered_map<Value, int, Value::ValueHasher> m_constant_check;
  std::vector<Value*> m_constants;
  int m_index = 0;
};
}  // Namespace linaro

#endif  // CONSTANT_POOL_H
