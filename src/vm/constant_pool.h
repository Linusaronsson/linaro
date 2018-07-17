#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include <vector>

#include "value.h"

namespace linaro {

/*
* A constant pool stores frequently used runtime Values.
*/
class ConstantPool {
private:
    int index = 0;
    //TODO: change to hash map (edit 2018-07-17: should it really be hash map?)
    std::vector<Value> constants;
public:
    int addIfNew(const Value& val);
    int addAlways(const Value& val);
    Value& operator[](int n) { return constants[n]; }
    int Size() const { return constants.size(); }
    std::vector<Value>& constants() { return constants; }
    int index() { return index; }
    void addConstant(Value& v) { constants.push_back(v); }
    Value& getConstant(int index) { return constants[index]; }
    void printConstantPool();
};

} // Namespace linaro

#endif // CONSTANT_POOL_H