#ifndef VM_H
#define VM_H

#include <stack>
#include <string>

//#include "../compiler/chunk.h"
#include "value.h"
#include "../linaro_utils/common.h"
#include "vm_context.h"
//#include "constant_pool.h" //if VM only has pointer to const pool, consider just doing a forward decl instead of this include. (class ConstantPool;)

class ConstantPool;
class BytecodeChunk;

namespace linaro {

enum VMEndingStatus : uint8_t {
    VM_SUCCESS,
    VM_COMPILE_ERR,
    VM_RUNTIME_ERR
};

class VM {
public:
    VM() { /* m_globals = new value[64]; */ }
    ~VM() {}
    int operandsize() const { return m_operand_stack.size(); }
    // create a vm instance from source file and execute
    VMEndingStatus interpret(const std::string& source, 
			const char* filename = nullptr);

    // execute from predefined vm environment (?)
    VMEndingStatus interpret(const VMContext& vm_context);

private:
    // runs main bytecodechunk initially. upon function calls, 
    // this will be called recursively for the function's bytecodechunk.
    VMEndingStatus execute(BytecodeChunk* code);
    //void call(functiondescriptor* fn);

    // extracting data from bytecode chunk
    //uint64_t read64bitoperand(bytecode* ip);
    uint32_t read32bitoperand(bytecode* ip);
    uint16_t read16bitoperand(bytecode* ip);
    uint8_t read8bitoperand(bytecode* ip);

    void binaryoperation(bytecode type);
    
    BytecodeChunk* m_main_code;
    ConstantPool* m_const_pool;
    Value* m_globals; //global variable space

    // for stacks: use top() to get element, then pop()
    // operand stack
    std::stack<Value> m_operand_stack;
    // call stack
    //std::stack<stackframe> m_call_stack;
};

} //Namespace Torus

#endif // VM_H
