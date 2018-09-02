#ifndef VM_H
#define VM_H

#include <stack>
#include <string>
#include <variant>

#include "../linaro_utils/common.h"
#include "../linaro_utils/utils.h"
#include "objects.h"
#include "vm_context.h"

class BytecodeChunk;

namespace Linaro {

struct StackFrame {
  StackFrame(Closure *closure) : closure{closure} {
    locals.resize(closure->fun()->numLocals());
  }

  uint32_t ip;
  Closure *closure;
  std::vector<Value> locals;
};

// Inspired by this: https://github.com/lua/lua/blob/master/lobject.h#L578
struct CapturedVariable {
  // Either points to the variable on stack (if enclosing function is still on
  // the CallStack). Otherwise it points to the closed value. The variable on
  // the stack is copied to 'closed' before it is popped of the stack.
  Value *val;

  // A captured variable is either opened or closed:
  // An open captured variable means that the value it refers to is still on
  // the call stack in some function's variable space.
  // All open captured variables are stored in a linked list in each thread.
  // When a captured variable is popped of the callstack it will become closed,
  // which means that it is removed from the linked list. Although the object
  // will remain alive because nested closures may still be using it.
  // Open: CapturedVariable* (linked list)
  // Closed: Value
  // std::variant<Value, CapturedVariable *> status;
  Value closed;
  // The closed variable
};

enum VMEndingStatus : uint8_t { VM_SUCCESS, VM_COMPILE_ERR, VM_RUNTIME_ERR };

class VM {
 public:
  VM() {}
  int operandStackSize() { return m_operand_stack.size(); }
  // Create a vm instance from source file and execute
  VMEndingStatus interpret(const std::string &source,
                           const char *filename = nullptr);

  // Execute from predefined vm environment (?)
  VMEndingStatus interpret(const VMContext &vm_context);

 private:
  void initVM();

  // Runs main bytecodechunk initially. Upon function calls,
  // this will be called recursively for the function's bytecodechunk.
  VMEndingStatus execute(BytecodeChunk *code);

  // Function call
  void call(const Value &v);

  // Extracting data from bytecode chunk
  inline uint8_t readByte();
  inline uint16_t read16BitOperand();
  inline uint32_t read32BitOperand();

  inline Function *getEnclosingFunction();

  // Get a constant from the constant pool of the currently running function
  inline Value &getConstant();

  // Get the constant pool of the currently running function.
  inline std::vector<Value> &getConstants();

  // Get the local variable space/captured variable of the StackFrame on the
  // top of the call stack.
  inline Value *getLocal(int i);
  inline Value *getCapturedVariable(int i);

  // Evaluating a binary operation
  void binaryOperation(Bytecode op);

  // Find the captured variable from the open captured variables
  CapturedVariable *captureVariable(int index);

  // Runtime error
  void runtimeError(const char *format, ...);

  // Top level chunk
  BytecodeChunk *m_main_code;

  // Code currently executing
  BytecodeChunk *m_current_chunk;

  // Instruction pointer into currently executing chunk
  uint32_t m_ip;

  // Global variable space
  Value *m_globals;

  // Operand stack
  Stack<Value> m_operand_stack;

  // Call stack
  Stack<StackFrame> m_call_stack;

  // This is where all open captured variables will be stored.
  // It should be a linked list.
  std::vector<CapturedVariable> m_open_captured_variables;
};

}  // namespace Linaro

#endif  // VM_H
