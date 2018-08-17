#ifndef VALUE_H
#define VALUE_H

#include <cstddef>
#include <iostream>
#include <memory>

namespace linaro {

enum ValueType {
  NUMBER,
  BOOLEAN,
  STRING,
  FUNCTION,
  ARRAY,
  //  OBJECT,
  THREAD,
  NOLL
};
class Function;
/*
 * Value represents any data type (dynamically typed).
 * Changes todo: Value cant be abstract if used in unorderd map.
 */
class Value {
public:
  Value() {}
  virtual ~Value() {}

  // Value interface
  virtual void print() const = 0;
  virtual size_t hash() const = 0;

  // Conversions/checking
  virtual bool canBeNumber() const { return false; }
  virtual double asNumber() const { return 0; }
  virtual bool asBoolean() const { return false; }
  virtual std::string asString() const { return "undefined"; }
  virtual std::shared_ptr<Function> asFunction() const { return nullptr; }

  // Required for c++ hash map.
  virtual bool operator==(const Value &lhs) = 0;
  struct ValueHasher {
    size_t operator()(const Value &v) const {
      return v.hash(); // defined in each value type class
    }
  };

  // Value helper methods

protected:
  ValueType m_type;
};

// Default value
class Undefined : public Value {
public:
  void print() const;

private:
};

class Number : public Value {
public:
  explicit Number();

  void print() const override { std::cout << m_val; }

  size_t hash() const override { return std::hash<double>{}(m_val); }

private:
  double m_val;
};

class String : public Value {};
class Function : public Value {};
class Array : public Value {};
class Thread : public Value {};

} // Namespace linaro

#endif // VALUE_H
