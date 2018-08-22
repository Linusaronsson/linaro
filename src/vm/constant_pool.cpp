#include "constant_pool.h"

namespace linaro {

int ConstantPool::addIfNew(const Value& val) {
  auto it = m_constant_check.find(val);
  if (it == m_constant_check.end()) {
    return addAlways(val);
  }
  return it->second;
}

int ConstantPool::addAlways(const Value& val) {
  auto it = m_constant_check.insert({val, m_index});
  m_constants.push_back(const_cast<Value*>(&it.first->first));
  return m_index++;
}

void ConstantPool::printConstantPool() {
  std::cout << "Constant pool:\n";
  for (size_t i = 0; i < size(); i++) {
    std::cout << i << ": " << *m_constants[i] << '\n';
  }
}

}  // namespace linaro
