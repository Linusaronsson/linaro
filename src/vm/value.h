#ifndef VALUE_H
#define VALUE_H

#include <cstddef>
#include <iostream>
#include <memory>
#include <variant>

namespace linaro {

#define OBJECTS(V) \
  V(Number) V(Boolean) V(String) V(Function) V(Array) V(Thread) V(Noll)

class Value;
class Object;
class String;
class Function;
class Array;
class Thread;

class Object {
 public:
  virtual ~Object() {}

  /* Linaro Object interface */

  // Printing
  virtual void print() const = 0;

  // Conversions/checking
  virtual bool canBeNumber() const = 0;
  virtual double asNumber() const = 0;
  virtual bool asBoolean() const = 0;
  virtual std::string asString() const = 0;

  // For c++ hash maps
  virtual size_t hash() const = 0;
  //  virtual bool operator==(const Object& lhs) = 0;
};

// class Number : public Object {
//  public:
//   explicit Number(double val) : m_val{val} {}
//   void print() const override { std::cout << m_val; }
//   size_t hash() const override { return std::hash<double>{}(m_val); }
//
//  private:
//   double m_val;
// };

class String : public Object {
 public:
  virtual void print() const override { std::cout << asString(); }
  // Conversions/checking
  virtual bool canBeNumber() const override { return false; }
  virtual double asNumber() const override { return 0; }
  virtual bool asBoolean() const override { return false; }
  virtual std::string asString() const override { return "placeholder"; }

  // For c++ hash maps
  virtual size_t hash() const override { return 0; }
};

class Function : public Object {
 public:
  virtual void print() const override { std::cout << asString(); }
  // Conversions/checking
  virtual bool canBeNumber() const override { return false; }
  virtual double asNumber() const override { return 0; }
  virtual bool asBoolean() const override { return false; }
  virtual std::string asString() const override { return "placeholder"; }

  // For c++ hash maps
  virtual size_t hash() const override { return 0; }

 private:
  bool is_compiled;  // for lazy compilation
};

class Array : public Object {
  virtual void print() const override { std::cout << asString(); }
  // Conversions/checking
  virtual bool canBeNumber() const override { return false; }
  virtual double asNumber() const override { return 0; }
  virtual bool asBoolean() const override { return false; }
  virtual std::string asString() const override { return "placeholder"; }

  // For c++ hash maps
  virtual size_t hash() const override { return 0; }
};

class Thread : public Object {
  virtual void print() const override { std::cout << asString(); }
  // Conversions/checking
  virtual bool canBeNumber() const override { return false; }
  virtual double asNumber() const override { return 0; }
  virtual bool asBoolean() const override { return false; }
  virtual std::string asString() const override { return "placeholder"; }

  // For c++ hash maps
  virtual size_t hash() const override { return 0; }
};

/* Linaro Value. Dynamically typed */
class Value {
 public:
#define V(type) n##type,
  enum ValueType : uint8_t { OBJECTS(V) };
#undef V

  // Initializing of different value type
  Value() : m_type{nNoll} {}
  Value(double d) : m_type{nNumber}, as{d} {}
  Value(bool b) : m_type{nBoolean}, as{b} {}
  Value(const String& s) : m_type{nString}, as{std::make_unique<String>(s)} {}
  Value(const Function& fun)
      : m_type{nFunction}, as{std::make_unique<Function>(fun)} {}
  Value(const Array& arr) : m_type{nArray}, as{std::make_unique<Array>(arr)} {}
  Value(const Thread& t) : m_type{nFunction}, as{std::make_unique<Thread>(t)} {}

  // Rule of 5
  Value(const Value& other);
  Value(Value&& other);
  Value& operator=(const Value& other);
  Value& operator=(Value&& other);

#define V(name)                                            \
  bool is##name() { return m_type == ValueType::n##name; } \
  OBJECTS(V)
#undef V

  bool isObject() const {
    return m_type != nNumber && m_type != nBoolean && m_type != nNoll;
  }

  template <typename T>
  const T& objectAs() const;

  void print() const;

  // Required for c++ hash map.
  //  virtual size_t hash() const = 0;
  //  virtual bool operator==(const Value& lhs) = 0;
  //  struct ValueHasher {
  //    size_t operator()(const Value& v) const {
  //      return v.hash();  // defined in each value type class
  //    }
  //  };

 private:
  ValueType m_type;
  std::variant<bool, double, std::unique_ptr<Object>> as;
};
}  // Namespace linaro

#endif  // VALUE_H
