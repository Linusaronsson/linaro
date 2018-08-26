#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>

#include <cstdarg>
#include <fstream>
#include <sstream>

#include "../parsing/token.h"
#include "common.h"

namespace linaro {

// Utility functions
std::string readFile(const char *filename);

// Utility classes
class Error {
 public:
  enum ErrorType : uint8_t {
    LexicalError = 0,
    SyntaxError,
    SemanticError,
    RuntimeError

  };

  constexpr static const char *const errorToString[4]{
      "[Lexical Error]:", "[Syntax Error]:", "[Semantic Error]",
      "[Runtime Error]:"};

 public:
  static void reportErrorAt(const Location &loc, ErrorType type,
                            const char *format, va_list args) {
    std::cerr << errorToString[(int)type] << loc << " ";
    vfprintf(stderr, format, args);
  }
};

class Value;

template <class T>
class Stack {
 public:
  inline void push(const T &val) { m_stack.push_back(val); }
  inline T &pop() {
    Value &val = m_stack.back();
    m_stack.pop_back();
    return val;
  }
  inline T &peek() { return m_stack.back(); }
  void reset() { m_stack.clear(); }
  size_t size() const { return m_stack.size(); }
  T &operator[](int i) { return m_stack[i]; }

 private:
  std::vector<T> m_stack;
};
}  // Namespace linaro

#endif  // UTILS_H
