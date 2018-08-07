#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include <vector>

#include "value.h"

namespace linaro {

/*
* A constant pool stores frequently used runtime Values.
*/
class ConstantPool {
public:
    int addIfNew(const Value& val);
    int addAlways(const Value& val);
    void printConstantPool();
    
    Value& operator[](int n) { return m_constants[n]; }
    int size() const { return m_constants.size(); }
    std::vector<Value>& constants() { return m_constants; }
    int index() { return m_index; }
    void addConstant(Value& v) { m_constants.push_back(v); }
    Value& getConstant(int index) { return m_constants[index]; }
private:
    int m_index = 0;
    //TODO: change to hash map (needed for addIfNew)
    std::vector<Value> m_constants;
};

} // Namespace linaro

#endif // CONSTANT_POOL_H