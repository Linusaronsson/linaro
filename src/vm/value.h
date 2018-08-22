#ifndef VALUE_H
#define VALUE_H

#include <cstddef>
#include <iostream>
#include <memory>
#include <variant>

#include "object.h"

namespace linaro {

#define VALUES(V) \
  V(Number) V(Boolean) V(String) V(Function) V(Array) V(Thread) V(Noll)

class Object;
class String;
class Function;
class Array;
class Thread;

/* Linaro Value. Dynamically typed */
class Value {
 public:
#define V(type) n##type,
  enum ValueType : uint8_t { VALUES(V) };
#undef V

  // Initializing of different value type
  Value() : m_type{nNoll} {}
  Value(double d) : m_type{nNumber}, as{d} {}
  Value(bool b) : m_type{nBoolean}, as{b} {}
  Value(const String& s) : m_type{nString}, as{std::make_shared<String>(s)} {}
  Value(const char* s) : m_type{nString}, as{std::make_shared<String>(s)} {}

  Value(const Function& fun)
      : m_type{nFunction}, as{std::make_shared<Function>(fun)} {}
  Value(const Array& arr) : m_type{nArray}, as{std::make_shared<Array>(arr)} {}
  Value(const Thread& t) : m_type{nFunction}, as{std::make_shared<Thread>(t)} {}
  virtual ~Value() {}

  // Helper methods
#define V(type) \
  bool is##type() const { return m_type == ValueType::n##type; }
  VALUES(V)
#undef V

  bool isObject() const {
    return m_type != nNumber && m_type != nBoolean && m_type != nNoll;
  }

  ValueType type() const { return m_type; }

  // Convert from Value to a reference to corresponding Object
  template <typename T>
  const T& valueTo() const {
    try {
      return static_cast<T&>(*std::get<std::shared_ptr<Object>>(as));
    } catch (std::bad_variant_access& e) {
      std::cerr << e.what();
      exit(1);
    }
  }

  bool canBeNumber() const;
  double asNumber() const;
  bool asBoolean() const;
  std::string asString() const;

  size_t hash() const;
  //  virtual bool operator==(const Value& lhs) = 0;
  //  struct ValueHasher {
  //    size_t operator()(const Value& v) const {
  //      return v.hash();  // defined in each value type class
  //    }
  //  };

  // Printing values
  void print() const { std::cout << asString(); }
  friend std::ostream& operator<<(std::ostream& s, const Value& v) {
    s << v.asString();
    return s;
  }

  /* Value comparisons */

  static bool numberEquals(double x, double y);
  static bool numberEquals(const Value& lhs, const Value& rhs);

  // todo: should probly be done with hash maps
  static bool stringEquals(const Value& lhs, const Value& rhs);

  // Value equality check. Supports implicit type conversion.
  static bool equal(const Value& lhs, const Value& rhs);

  // Will always return false if type isnt equal. Used for constant pool.
  // Does the same as Equal at the moment, will change though.
  static bool strictEqual(const Value& lhs, const Value& rhs);

  // Relational comparison
  enum cmp_result { eq, lt, gt, undefined };
  static cmp_result compare(const Value& lhs, const Value& rhs);

  // Value arithmetic
  Value operator+(const Value& other);
  Value operator-(const Value& other);
  Value operator/(const Value& other);
  Value operator%(const Value& other);
  Value operator*(const Value& other);
  Value operator-() const;

  static Value power(Value& other, const Value& rhs);

  String* ggStr() {
    return static_cast<String*>(std::get<std::shared_ptr<Object>>(as).get());
  }

 private:
  ValueType m_type;
  std::variant<bool, double, std::shared_ptr<Object>> as;
};
}  // Namespace linaro

#endif  // VALUE_H
