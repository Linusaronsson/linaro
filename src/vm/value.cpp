#include "value.h"

#include <math.h>
#include <cassert>

#include "../linaro_utils/logging.h"
#include "object.h"

namespace linaro {

/* Value */

// Deep copy (currently not used)
// Value::Value(const Value& other) : m_type{other.m_type} {
//   std::cout << "REACHED DEF CON";
//   switch (other.m_type) {
//     case nNumber:
//       as = std::get<double>(other.as);
//       break;
//     case nBoolean:
//       as = std::get<bool>(other.as);
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
    case nNumber:
      return std::hash<double>{}(std::get<double>(as));
    case nBoolean:
      return std::hash<bool>{}(std::get<bool>(as));
    case nNoll:
      return 0;  // ?
    default:
      return std::get<std::shared_ptr<Object>>(as)->hash();
  }
  UNREACHABLE();
  return 0;
}

bool Value::canBeNumber() const {
  switch (m_type) {
    case nNumber:
    case nBoolean:
      return true;
    case nNoll:
      return false;
    default:
      // assert(std::holds_alternative<std::shared_ptr<Object>>(as));
      return std::get<std::shared_ptr<Object>>(as)->canBeNumber();
  }
  UNREACHABLE();
  return false;
}

double Value::asNumber() const {
  switch (m_type) {
    case nNumber:
      return std::get<double>(as);
    case nBoolean:
      return std::get<bool>(as);
    case nNoll:
      return 0.0;
    default:
      // assert(std::holds_alternative<std::shared_ptr<Object>>(as));
      return std::get<std::shared_ptr<Object>>(as)->asNumber();
  }
  UNREACHABLE();
  return 0.0;
}

bool Value::asBoolean() const {
  switch (m_type) {
    case nNumber:
      return std::get<double>(as);
    case nBoolean:
      return std::get<bool>(as);
    case nNoll:
      return false;
    default:
      // assert(std::holds_alternative<std::shared_ptr<Object>>(as));
      return std::get<std::shared_ptr<Object>>(as)->asBoolean();
  }
  UNREACHABLE();
  return false;
}

std::string Value::asString() const {
  switch (m_type) {
    case nNumber:
      return std::to_string(std::get<double>(as));
    case nBoolean:
      return (std::get<bool>(as) ? "true" : "false");
    case nNoll:
      return "Undefined";
    default:
      // assert(std::holds_alternative<std::shared_ptr<Object>>(as));
      return std::get<std::shared_ptr<Object>>(as)->asString();
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

Value Value::operator+(const Value& other) {
  if (isNoll() || other.isNoll()) {
    return Value();  // Undefined
  }

  if (canBeNumber() && other.canBeNumber() && !isString() &&
      !other.isString()) {
    return Value(asString() + other.asString());
  }
  return Value(asNumber() + other.asNumber());
}

#define bin_op(op) this->asNumber() op other.asNumber()

Value Value::operator-(const Value& other) {
  if (isNoll() || other.isNoll()) {
    return Value();  // Undefined
  }
  return Value(bin_op(-));
}

Value Value::operator/(const Value& other) {
  if (isNoll() || other.isNoll()) {
    return Value();  // Undefined
  }
  return Value(bin_op(/));
}
// modulo not working for doubles? find out
Value Value::operator%(const Value& other) {
  if (isNoll() || other.isNoll()) {
    return Value();  // Undefined
  }
  return Value(fmod(this->asNumber(), other.asNumber()));
}

Value Value::operator*(const Value& other) {
  if (isNoll() || other.isNoll()) {
    return Value();  // Undefined
  }
  return Value(bin_op(*));
}

Value Value::power(Value& lhs, const Value& rhs) {
  if (lhs.isNoll() || rhs.isNoll()) {
    return Value();  // Undefined
  }
  return Value(pow(lhs.asNumber(), rhs.asNumber()));
}

bool Value::numberEquals(double x, double y) {
  if (isnan(x)) return false;
  if (isnan(y)) return false;
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
    case nNoll:
      return true;  // change?
    case nBoolean:
    case nNumber:
      return numberEquals(lhs, rhs);
    case nString:
    case nArray:
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
