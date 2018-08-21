#include <cassert>

#include "value.h"

namespace linaro {

// Value

Value::Value(const Value& other) : m_type{other.m_type} {}

template <typename T>
const T& Value::objectAs() const {
  try {
    return static_cast<T>(*(std::get<std::unique_ptr<Object>>(as)));
  } catch (std::bad_variant_access&) {
    std::cerr << "Bad variant access";
    exit(1);
  }
}

void Value::print() const {
  switch (m_type) {
    case nNumber:
      // assert(std::hold_alternative<double>(as));
      std::cout << std::get<double>(as);
      break;
    case nBoolean:
      // assert(std::hold_alternative<double>(as));
      std::cout << (std::get<bool>(as) ? "true" : "false");
      break;
    case nNoll:
      std::cout << "Undefined";
      break;
    default:
      // assert(std::holds_alternative<std::unique_ptr<Object>>(as));
      std::get<std::unique_ptr<Object>>(as)->print();
  }
}

}  // namespace linaro
