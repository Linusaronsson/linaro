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

int Function::addCapturedVariable(int index, bool is_local) {
  for (int i = 0; i < num_captured_variables; i++) {
    if (index == m_captured_variables[i].index &&
        is_local == m_captured_variables[i].is_local) {
      return i;
    }
  }
  m_captured_variables.push_back({index, is_local});
  return num_captured_variables++;
}

#ifdef DEBUG
void Function::printCapturedVariables() {
  std::cout << "Captured variables:\n";
  for (const auto& ccv : m_captured_variables) {
    std::cout << "index: " << ccv.index << " is_local: " << ccv.is_local
              << '\n';
  }
}

void Function::printConstants() const {
  std::cout << "Constant pool:\n";
  for (size_t i = 0; i < m_constants.size(); i++) {
    std::cout << i << ": " << m_constants[i] << '\n';
  }
}
#endif

/* Array */

// Array::Array(std::initializer_list<Value> list)
//   : m_values{list.begin(), list.end()} {}

// Using this impl for now:
size_t Array::hash() const {
  size_t seed = m_values.size();
  if (seed > 1) {
    for (auto& i : m_values) {
      seed ^= i.hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
  return m_values[0].hash();
}

double Array::asNumber() const { return m_values[0].asNumber(); }
bool Array::asBoolean() const { return m_values[0].asNumber(); }

std::string Array::asString() const {
  std::string res;
  for (const auto& v : m_values) {
    res = res + v.asString();  //+ delimiter;
  }
  return res;
}

/* Thread (TODO) */

}  // Namespace linaro
