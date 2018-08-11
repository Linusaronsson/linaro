#ifndef VALUE_H
#define VALUE_H

#include <cstddef>
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
 
    // Value helper methods (overridden if they can be applied)
    virtual  bool canBeNumber() const { return false; }
    virtual double asNumber() const { return 0; }
    virtual bool asBoolean() const { return false; }
    virtual std::string asString() const { return "undefined"; }
    virtual std::shared_ptr<Function> asFunction() const { return nullptr; }

    // Required for c++ hash map.
    virtual bool operator==(const Value& lhs) = 0;
    struct ValueHasher {
        size_t operator()(const Value& v) const {
            return v.hash(); //defined in each value type class
        }
    }; 
    
    // Value helper methods

protected:
    ValueType m_type;
};

class Number : public Value { 
public:
    explicit Number();
    void print() const override;    
    size_t hash() const override;
    
private:
    double m_val;
};


class String : public Value {};
class Function : public Value {};
class Array : public Value {};
class Thread : public Value {};




} // Namespace linaro

#endif // VALUE_H
