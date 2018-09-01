#ifndef VALUE_H
#define VALUE_H

#include <cstddef>
#include <iostream>
#include <memory>
#include <variant>

#include "../linaro_utils/common.h"

namespace Linaro {

#define VALUES(V) V(Number) V(Boolean) V(Object) V(Undefined) V(Noll)
#define OBJECTS(O) O(String) O(Function) O(Array) O(Closure) O(Thread)

#define V(type) n##type,
enum class ValueType : uint8_t { VALUES(V) };
#undef V

#define O(type) n##type,
enum ObjectType : uint8_t { OBJECTS(O) };
#undef O

class Object {
 public:
  Object(ObjectType type) : m_type{type} {}
  virtual ~Object() {}

  // Helper methods
#define O(type) \
  inline bool is##type() const { return m_type == ObjectType::n##type; }
  OBJECTS(O)
#undef O

  /* Object interface */

  // Conversions/checking
  virtual bool canBeNumber() const { return false; };
  virtual double asNumber() const { return 0; };
  virtual bool asBoolean() const { return false; };
  virtual std::string asString() const { return "Undefined"; };
  virtual size_t hash() const = 0;

 private:
  ObjectType m_type;
};

/* Linaro Value. Dynamically typed */
class Value {
 public:
  // Initializing of different value type
  // NOTE: The old constructors (with std::make_shared) could maybe be brought
  // back if they are put in value.cpp instad? try it.
  Value() : m_type{ValueType::nUndefined} {}
  Value(ValueType type) : m_type{type} { CHECK(m_type == ValueType::nNoll); }
  Value(double d) : m_type{ValueType::nNumber}, as{d} {}
  Value(bool b) : m_type{ValueType::nBoolean}, as{b} {}
  Value(std::shared_ptr<Object>&& obj) : m_type{ValueType::nObject}, as{obj} {
    CHECK(obj != nullptr);
  }

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
#undef O
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

}  // namespace Linaro

#endif  // VALUE_H
