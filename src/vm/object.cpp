#include "object.h"

#include "value.h"

namespace linaro {

/* String */

bool String::canBeNumber() const {
  for (char c : m_str) {
    if (c <= '0' || c >= '9') {
      if (c == '.') {
        continue;
      }
      return false;
    }
  }
  return true;
}

double String::asNumber() const {
  double temp;
  try {
    temp = std::stod(m_str);
  } catch (const std::invalid_argument& err) {
    // TODO: THROW RUNTIME ERROR.
  }
  return temp;
}

/* Function */

/* Array */

// Array::Array(std::initializer_list<Value> list)
//   : m_values{list.begin(), list.end()} {}

// Using this impl for now:
size_t Array::hash() const {
  std::size_t seed = m_values.size();
  for (auto& i : m_values) {
    seed ^= i.hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  return seed;
}

double Array::asNumber() const { return m_values[0].asNumber(); }
bool Array::asBoolean() const { return m_values[0].asNumber(); }


std::string Array::asString() const {
  std::string res;
  for (const auto& v : m_values) {
    res = res + v.asString() + delimiter;
  }
  return res;
}

/* Thread (TODO) */

}  // Namespace linaro