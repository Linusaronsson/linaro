#ifndef VALUE_H
#define VALUE_H

#include <cstddef>
#include <iostream>
#include <memory>
#include <variant>

#include "object.h"

#define VALUES(V) V(Number) V(Boolean) V(Object) V(Undefined) V(Noll)

namespace linaro {
/* Linaro Value. Dynamically typed */
class Value {
 public:
#define V(type) n##type,
  enum ValueType : uint8_t { VALUES(V) };
#undef V

  // Initializing of different value type
  Value() : m_type{nUndefined} {}
  Value(ValueType type) : m_type{type} {}
  Value(double d) : m_type{nNumber}, as{d} {}
  Value(bool b) : m_type{nBoolean}, as{b} {}
  Value(const char* s) : m_type{nObject}, as{std::make_shared<String>(s)} {}
  Value(std::shared_ptr<Object> obj) : m_type{nObject}, as{obj} {}

  Value(const String& s) : m_type{nObject}, as{std::make_shared<String>(s)} {}
  Value(const Function& fun)
      : m_type{nObject}, as{std::make_shared<Function>(fun)} {}
  Value(const Closure& cl)
      : m_type{nObject}, as{std::make_shared<Closure>(cl)} {}
  Value(const Array& arr) : m_type{nObject}, as{std::make_shared<Array>(arr)} {}
  Value(const Thread& t) : m_type{nObject}, as{std::make_shared<Thread>(t)} {}
  virtual ~Value() {}

  // Helper methods

  // Checks if the value is of a given primitive type
#define V(type) \
  inline bool is##type() const { return m_type == ValueType::n##type; }
  VALUES(V)
#undef V

  // Checks if the value is of a given object type.
#define O(type) \
  inline bool is##type() const { return isObject() && AS_OBJ()->is##type(); }
  OBJECTS(O)
#undef V

  ValueType type() const { return m_type; }

  // Convert from Value to a reference to corresponding Object
  template <typename T>
  inline T& valueTo() const {
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

  // Printing values
  inline void print() const { std::cout << asString(); }
  friend std::ostream& operator<<(std::ostream& s, const Value& v) {
    s << v.asString();
    return s;
  }

  /* Value comparisons */

  static inline bool numberEquals(double x, double y);
  static inline bool numberEquals(const Value& lhs, const Value& rhs);
  static inline bool stringEquals(const Value& lhs, const Value& rhs);

  // Value equality check. Supports implicit type conversion.
  static bool equal(const Value& lhs, const Value& rhs);

  // Will always return false if type isn't equal. Used for constant pool.
  // Does the same as Equal at the moment, will change though.
  static bool strictEquals(const Value& lhs, const Value& rhs);

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

  static Value power(Value& lhs, const Value& rhs);

  // Hashing a value
  size_t hash() const;

  // For c++ hash maps.
  virtual bool operator==(const Value& lhs) const {
    return strictEquals(*this, lhs);
  }

  struct ValueHasher {
    size_t operator()(const Value& v) const noexcept {
      return v.hash();  // defined in each value type class
    }
  };

 private:
  inline double AS_NUMBER() const { return std::get<double>(as); }
  inline bool AS_BOOL() const { return std::get<bool>(as); }
  inline std::shared_ptr<Object> AS_OBJ() const {
    return std::get<std::shared_ptr<Object>>(as);
  }

  ValueType m_type;
  std::variant<bool, double, std::shared_ptr<Object>> as;
};
}  // Namespace linaro

#endif  // VALUE_H
