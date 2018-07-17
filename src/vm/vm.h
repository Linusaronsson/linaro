#ifndef VM_H
#define VM_H

#include <stack>

#include "../compiler/chunk.h"
#include "../linaro_utils/common.h"
#include "value.h"
#include "constant_pool.h"

namespace Torus {

class VM {
public:
    VM() {}
    ~VM() {}
    int operandSize() { return operand_stack.size(); }
    void interpret(std::string source, const char* filename = nullptr);
private:
    BytecodeChunk* main_code;
    ConstantPool* const_pool;
    std::stack<Value> globals; //use top() to get element, then pop()

    int sp = 0;
    std::stack<Value> operand_stack;

    uint32_t read32bitOperand(uint8_t* ip);
    void binaryOperation(uint8_t type);

    //void Call(FunctionDescriptor* fn);
    
    //runs main BytecodeChunk initially. Upon function calls, 
    //this will be called recursively for the function's BytecodeChunk.
    void execute(BytecodeChunk* code);
};
} //Namespace Torus

#endif // VM_H