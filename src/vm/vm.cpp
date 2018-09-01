#include "vm.h"

#include "../code_generator/code_generator.h"

namespace Linaro {

VMEndingStatus VM::interpret(const std::string& source, const char* filename) {
  // initialize VM (todo)

  // run the code
  VMEndingStatus res = execute(main_code);

  // turn off vm (todo)
  delete m_globals;

  // return status code
  return res;
}

void VM::initVM() {}

void VM::runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  const Location& loc = current_chunk->getLocation(ip);
  Error::reportErrorAt(loc, Error::RuntimeError, format, args);
  va_end(args);

  // Do stuff to reset VM
  m_operand_stack.reset();
  m_call_stack.reset();
}

uint8_t VM::readByte() { return current_chunk->readByte(ip++); }
uint16_t VM::read16Bits() { return readByte() | (readByte() << 8); }
uint32_t VM::read32Bits() { return read16Bits() | (read16Bits() << 16); }

Function* VM::getEnclosingFunction() {
  return m_call_stack.peek().m_closure->fun();
}

std::vector<Value>& VM::getConstants() {
  return getEnclosingFunction()->constants();
}

Value VM::getConstant() { return getConstants()[read16Bits()]; }
std::vector<Value>& VM::getLocals() { return m_call_stack.peek().locals; }

CapturedVariable* VM::captureVariable(int index) {
  // Get a pointer to the value we are trying to capture from the local space of
  // the function on the top of the callstack.
  Value* val = &getConstants()[index];

  int num_captured = getEnclosingFunction()->numCapturedVariables();
  int num_open_vars = open_captured_variables.size();
  // Go through the list of open captured variables and see if this
  // variable has already been captured by some other closure, if so then reuse
  // it. No need to loop more than 'num_captured', since we are only trying to
  // capture variables of the closest enclosing function.
  // Correction here: only have to loop for 'index' times right? (actually maybe
  // not)
  for (int i = 0; (i <= num_captured) && (i < num_open_vars); i++) {
    // If the value we are trying to capture points to the same value as this
    // already open captured variable, then reuse this one. This means that this
    // closure and the closure that already captured it will now point to the
    // same variable even if the local variable space goes off the stack and
    // closes the open variables.
    if (open_captured_variables[i].val == val)
      return &open_captured_variables[i];
  }

  // The variable has not yet been captured, create a new one.
  CapturedVariable cv;
  // Have it point to the captured variable;
  cv.val = val;
  // Add it to the list of open captured variables
  open_captured_variables.push_back(cv);
  return &open_captured_variables.back();
}

void VM::binaryOperation(Bytecode type) {}

VMEndingStatus VM::execute(BytecodeChunk* code) {
  current_chunk = code;
  ip = 0;
  for (;;) {
    Bytecode op = static_cast<Bytecode>(readByte());
    switch (op) {
      case Bytecode::nop:
        break;
      case Bytecode::closure: {
        // Extract function from constant pool
        Value v = getConstant();
        CHECK(v.isFunction());
        Function& fn = v.valueTo<Function>();
        // Construct a closure from this function
        auto closure = std::make_shared<Closure>(Closure(&fn));
        // Initialize the captured variables of this closure
        for (int i = 0; i < fn.numCapturedVariables(); i++) {
          // Extract the compile-time captured variable from the function
          // object.
          CompilerCapturedVariable* compiler_captured =
              fn.getCapturedVariable(i);

          // Now use that to appropriately construct a runtime version of that
          // captured variable.
          if (compiler_captured->is_local) {
            // Local means that it was captured from the immediately enclosing
            // function, so we have to capture it ourselves. In this case the
            // index points into the local space of the enclosing function
            // (which is now on top of callstack).
            closure->addCapturedVariable(
                captureVariable(compiler_captured->index));
          } else {
            // A non local captured variable has already been captured by the
            // enclosing function, so simply reuse it. In this case the index
            // points into the list of captured variables of the enclosing
            // closure.
            StackFrame& tos = m_call_stack.peek();
            std::vector<CapturedVariable*>& cv_tos =
                tos.m_closure->getCapturedVariables();
            closure->addCapturedVariable(cv_tos[compiler_captured->index]);
          }
        }
        // The captured variables are now pointing to the right place, push the
        // closure to the operand stack.
        m_operand_stack.push(Value(closure));
      } break;
      default:
        UNREACHABLE();
    }
    return VMEndingStatus::VM_SUCCESS;
  }
}

}  // namespace Linaro
