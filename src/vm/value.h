#ifndef VALUE_H
#define VALUE_H

#include <cstddef>

namespace linaro {

enum ValueType {
    NUMBER,
    STRING,
    FUNCTION,
    ARRAY,
//  OBJECT,
    THREAD,
    NOLL
};

/*
 * Value represents any data type (dynamically typed).
 * Changes todo: Value cant be abstract if used in unorderd map. 
 */
class Value {
public:
    virtual void print() const = 0;
    virtual size_t hash() const = 0;

    // Required for c++ hash map.
    virtual bool operator==(const Value& lhs) = 0;
    struct ValueHasher {
        size_t operator()(const Value& v) const {
            return v.hash(); //defined in each value type class
        }
    }; 

    

private:
    ValueType m_type;
    double m_value;
};

class Number : public Value {};
class String : public Value {};
class Function : public Value {};
class Array : public Value {};
class Thread : public Value {};




} // Namespace linaro

#endif // VALUE_H
