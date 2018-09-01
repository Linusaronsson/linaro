#include "objects.h"

#include "../ast/expression.h"
#include "../ast/statement.h"
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
  for (int i = 0; i < m_num_captured_variables; i++) {
    if (index == m_captured_variables[i].index &&
        is_local == m_captured_variables[i].is_local) {
      return i;
    }
  }
  m_captured_variables.push_back({index, is_local});
  return m_num_captured_variables++;
}

#ifdef DEBUG
void Function::printCapturedVariables() const {
  std::cout << "Captured variables:\n";
  for (const auto& ccv : m_captured_variables) {
    std::cout << "index: " << ccv.index << " is_local: " << ccv.is_local
              << '\n';
  }
}

void Function::printFunction() {
  if (m_fn_ast->isTopLevel()) {
    std::cout << "\n---- PROGRAM DEBUG ----\n\n";
  }

  std::cout << "fn " << m_name << "(";
  const auto& args = m_fn_ast->args();
  for (auto it = args.begin(); it != args.end(); it++) {
    std::cout << it->name();
    if (std::next(it) != args.end()) std::cout << ", ";
  }
  std::cout << "):\n";
  std::cout << "Num arguments: " << m_num_args
            << "\nNum locals: " << m_num_locals
            << "\nNum captured variables: " << m_num_captured_variables << '\n';
  printCapturedVariables();
  printConstants();

  std::cout << "Bytecode: \n";
  disassembleChunk();
  std::cout << '\n';
}

void Function::printConstants() {
  std::cout << "Constant pool (" << m_name << "):\n";

  for (size_t i = 0; i < m_constants.size(); i++) {
    Value val = m_constants[i];
    if (val.isFunction()) {
      std::cout << '\n' << i << ": ";
      val.valueTo<Function>().printFunction();
    } else {
      std::cout << i << ": ";
      std::cout << val << '\n';
    }
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
      seed ^= i.second.hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
  return const_cast<Array*>(this)->get(0.0).hash();
}

double Array::asNumber() const {
  return const_cast<Array*>(this)->get(0.0).asNumber();
}
bool Array::asBoolean() const {
  return const_cast<Array*>(this)->get(0.0).asBoolean();
}

std::string Array::asString() const {
  std::string res;
  for (const auto& v : m_values) {
    res = res + v.second.asString();  //+ delimiter;
  }
  return res;
}

/* Thread (TODO) */

}  // Namespace linaro
