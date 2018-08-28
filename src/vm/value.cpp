#include "value.h"

#include <math.h>
#include <cassert>
#include <sstream>

#include "../linaro_utils/common.h"
#include "object.h"

namespace linaro {

/* Value */

// Deep copy (currently not used)
// Value::Value(const Value& other) : m_type{other.m_type} {
//   std::cout << "REACHED DEF CON";
//   switch (other.m_type) {
//     case nNumber:
//       as = AS_NUMBER()(other.as);
//       break;
//     case nBoolean:
//       as = AS_BOOL()(other.as);
//       break;
//     case nNoll:
//       return;
//     case nString:
//       as = std::make_shared<String>(other.valueTo<String>());
//       break;
//     case nFunction:
//       as = std::make_shared<Function>(other.valueTo<Function>());
//       break;
//     case nArray:
//       as = std::make_shared<Array>(other.valueTo<Array>());
//       break;
//     case nThread:
//       as = std::make_shared<Thread>(other.valueTo<Thread>());
//       break;
//   }
// }

size_t Value::hash() const {
  switch (m_type) {
    case ValueType::nNumber:
      return std::hash<double>{}(AS_NUMBER());
    case ValueType::nBoolean:
      return std::hash<bool>{}(AS_BOOL());
    case ValueType::nNoll:
    case ValueType::nUndefined:
      return 0;  // ?
    default:
      return AS_OBJ()->hash();
  }
  UNREACHABLE();
  return 0;
}

bool Value::canBeNumber() const {
  switch (m_type) {
    case ValueType::nNumber:
    case ValueType::nBoolean:
      return true;
    case ValueType::nNoll:
    case ValueType::nUndefined:
      return false;
    default:
      // assert(std::holds_alternative<std::shared_ptr<Object>>(as));
      return AS_OBJ()->canBeNumber();
  }
  UNREACHABLE();
  return false;
}

double Value::asNumber() const {
  switch (m_type) {
    case ValueType::nNumber:
      return AS_NUMBER();
    case ValueType::nBoolean:
      return static_cast<double>(AS_BOOL());
    case ValueType::nNoll:
    case ValueType::nUndefined:
      return 0.0;
    default:
      // assert(std::holds_alternative<std::shared_ptr<Object>>(as));
      return AS_OBJ()->asNumber();
  }
  UNREACHABLE();
  return 0.0;
}

bool Value::asBoolean() const {
  switch (m_type) {
    case ValueType::nNumber:
      return static_cast<bool>(AS_NUMBER());
    case ValueType::nBoolean:
      return AS_BOOL();
    case ValueType::nNoll:
    case ValueType::nUndefined:
      return false;
    default:
      // assert(std::holds_alternative<std::shared_ptr<Object>>(as));
      return AS_OBJ()->asBoolean();
  }
  UNREACHABLE();
  return false;
}

std::string Value::asString() const {
  switch (m_type) {
    case ValueType::nNumber: {
      std::ostringstream ss;
      ss << AS_NUMBER();
      return ss.str();
    }
    case ValueType::nBoolean:
      return (AS_BOOL() ? "true" : "false");
    case ValueType::nNoll:
    case ValueType::nUndefined:
      return "Undefined";
    default:
      // assert(std::holds_alternative<std::shared_ptr<Object>>(as));
      return AS_OBJ()->asString();
  }
  UNREACHABLE();
  return nullptr;
}

Value Value::operator-() const {
  if (isNumber()) {
    return Value(-asNumber());
  }
  return Value();
}

#define CHECK_FOR_NULL_VAL()        \
  if (isNoll() || other.isNoll()) { \
    return Value();                 \
  }

Value Value::operator+(const Value& other) {
  CHECK_FOR_NULL_VAL()
  if (canBeNumber() && other.canBeNumber() && !isString() &&
      !other.isString()) {
    return Value(asString() + other.asString());
  }
  return Value(asNumber() + other.asNumber());
}

#define bin_op(op) this->asNumber() op other.asNumber()

Value Value::operator-(const Value& other) {
  CHECK_FOR_NULL_VAL()
  return Value(bin_op(-));
}

Value Value::operator/(const Value& other) {
  CHECK_FOR_NULL_VAL()
  return Value(bin_op(/));
}
// modulo not working for doubles? find out
Value Value::operator%(const Value& other) {
  CHECK_FOR_NULL_VAL()
  return Value(fmod(this->asNumber(), other.asNumber()));
}

Value Value::operator*(const Value& other) {
  CHECK_FOR_NULL_VAL()
  return Value(bin_op(*));
}

Value Value::power(Value& lhs, const Value& rhs) {
  if (lhs.isNoll() || rhs.isNoll()) {
    return Value();  // Undefined
  }
  return Value(pow(lhs.asNumber(), rhs.asNumber()));
}

bool Value::numberEquals(double x, double y) {
  if (std::isnan(x) || std::isnan(y)) return false;
  return x == y;
}

bool Value::numberEquals(const Value& lhs, const Value& rhs) {
  return numberEquals(lhs.asNumber(), rhs.asNumber());
}

bool Value::stringEquals(const Value& lhs, const Value& rhs) {
  return lhs.asString() == rhs.asString();
}

bool Value::equal(const Value& lhs, const Value& rhs) {
  ValueType temp = lhs.type();
  switch (temp) {
    case ValueType::nNoll:
    case ValueType::nUndefined:
      return false;
    case ValueType::nBoolean:
    case ValueType::nNumber:
      return numberEquals(lhs, rhs);
    case ValueType::nObject:
      return stringEquals(lhs, rhs);
    default:
      UNREACHABLE();
  }

  // for now unreachable
  UNREACHABLE();
  return false;
}

bool Value::strictEquals(const Value& lhs, const Value& rhs) {
  if (lhs.type() != rhs.type()) return false;
  return equal(lhs, rhs);
}

Value::cmp_result Value::compare(const Value& lhs, const Value& rhs) {
  if (!lhs.canBeNumber() && !rhs.canBeNumber()) {
    return cmp_result::undefined;
  }

  double result = lhs.asNumber() - rhs.asNumber();
  if (result > 0) {
    return cmp_result::gt;
  } else if (result < 0) {
    return cmp_result::lt;
  } else {
    return cmp_result::eq;
  }
}

}  // namespace linaro
