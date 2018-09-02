#include "vm.h"

#include "../code_generator/code_generator.h"

namespace Linaro {

VMEndingStatus VM::interpret(const std::string& source, const char* filename) {
  // initialize VM (todo)

  // run the code
  VMEndingStatus res = execute(m_main_code);

  // turn off vm (todo)
  delete m_globals;

  // return status code
  return res;
}

void VM::initVM() {}

void VM::runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  const Location& loc = m_current_chunk->getLocation(m_ip);
  Error::reportErrorAt(loc, Error::RuntimeError, format, args);
  va_end(args);

  // Do stuff to reset VM
  m_operand_stack.reset();
  m_call_stack.reset();
}

uint8_t VM::readByte() { return m_current_chunk->readByte(m_ip++); }
uint16_t VM::read16BitOperand() { return readByte() | (readByte() << 8); }
uint32_t VM::read32BitOperand() {
  return read16BitOperand() | (read16BitOperand() << 16);
}

Function* VM::getEnclosingFunction() {
  return m_call_stack.peek().closure->fun();
}

std::vector<Value>& VM::getConstants() {
  return getEnclosingFunction()->constants();
}

Value& VM::getConstant() { return getConstants()[read16BitOperand()]; }
Value* VM::getLocal(int i) { return &m_call_stack.peek().locals[i]; }
Value* VM::getCapturedVariable(int i) {
  return m_call_stack.peek().closure->getCapturedVariables()[i]->val;
}

CapturedVariable* VM::captureVariable(int index) {
  // Get a pointer to the value we are trying to capture from the local space of
  // the function on the top of the callstack.
  Value* val = &getConstants()[index];

  int num_captured = getEnclosingFunction()->numCapturedVariables();
  int num_open_vars = m_open_captured_variables.size();
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
    if (m_open_captured_variables[i].val == val)
      return &m_open_captured_variables[i];
  }

  // The variable has not yet been captured, create a new one.
  CapturedVariable cv;
  // Have it point to the captured variable;
  cv.val = val;
  // Add it to the list of open captured variables
  m_open_captured_variables.push_back(cv);
  return &m_open_captured_variables.back();
}

void VM::binaryOperation(Bytecode op) {
  Value op2 = m_operand_stack.pop();
  Value op1 = m_operand_stack.pop();
  Value result;
  switch (op) {
    case Bytecode::add:
      result = op1 + op2;
      break;
    case Bytecode::sub:
      result = op1 - op2;
      break;
    case Bytecode::mul:
      result = op1 * op2;
      break;
    case Bytecode::div:
      result = op1 / op2;
      break;
    case Bytecode::mod:
      result = op1 % op2;
      break;
    case Bytecode::exp:
      result = Value::power(op1, op2);
      break;
    case Bytecode::gt: {
      Value::cmp_result res = Value::compare(op1, op2);
      if (res == Value::cmp_result::gt)
        result = true;
      else
        result = false;
      break;
    }
    case Bytecode::lt: {
      Value::cmp_result res = Value::compare(op1, op2);
      if (res == Value::cmp_result::lt)
        result = true;
      else
        result = false;
      break;
    }
    case Bytecode::gte: {
      Value::cmp_result res = Value::compare(op1, op2);
      if (res == Value::cmp_result::gt || res == Value::cmp_result::eq)
        result = true;
      else
        result = false;
      break;
    }
    case Bytecode::lte: {
      Value::cmp_result res = Value::compare(op1, op2);
      if (res == Value::cmp_result::lt || res == Value::cmp_result::eq)
        result = true;
      else
        result = false;
      break;
    }
    case Bytecode::eq:
      result = Value::equal(op1, op2);
      break;
    case Bytecode::neq:
      result = !Value::equal(op1, op2);
      break;
    default:
      UNREACHABLE();
  }
  m_operand_stack.push(result);
}

void VM::call(const Value& v) {
  Closure& closure = v.valueTo<Closure>();
  m_call_stack.push(StackFrame(&closure));
  for (int i = 0; i < closure.fun()->numArgs(); i++) {
    *getLocal(i) = m_operand_stack.peek();
    m_operand_stack.pop_back();
  }
  execute(closure.fun()->code());
}

VMEndingStatus VM::execute(BytecodeChunk* _code) {
  m_current_chunk = _code;
  const std::vector<uint8_t>& code = _code->code();
  for (;;) {
    Bytecode op = static_cast<Bytecode>(readByte());
    switch (op) {
      case Bytecode::nop:
        break;
      case Bytecode::pop:
        CHECK(m_operand_stack.size() > 0);
        m_operand_stack.pop();
        break;
      case Bytecode::dup:
        m_operand_stack.push(m_operand_stack.peek());
        break;
      case Bytecode::incr:
        m_operand_stack.peek() = m_operand_stack.peek() + 1.0;
        // Needed?
        // m_operand_stack.pop_back();
        break;
      case Bytecode::decr:
        m_operand_stack.peek() = m_operand_stack.peek() - 1.0;
        // Needed?
        // m_operand_stack.pop_back();
        break;
      case Bytecode::add:
      case Bytecode::sub:
      case Bytecode::mod:
      case Bytecode::mul:
      case Bytecode::div:
      case Bytecode::exp:
      case Bytecode::neq:
      case Bytecode::eq:
      case Bytecode::lt:
      case Bytecode::lte:
      case Bytecode::gt:
      case Bytecode::gte:
        binaryOperation(op);
        break;
      case Bytecode::neg:
        m_operand_stack.peek() = -m_operand_stack.peek().asNumber();
        break;
      case Bytecode::NOT:
        m_operand_stack.peek() = !m_operand_stack.peek().asBoolean();
        break;
      case Bytecode::to_bool:
        m_operand_stack.peek() = m_operand_stack.peek().asBoolean();
        break;
      case Bytecode::jmp:
        m_ip = read16BitOperand();
        break;
      case Bytecode::jmp_true:
        if (m_operand_stack.peek().asBoolean()) {
          // TOS was true, make the jump.
          m_ip = read16BitOperand();

          // Resolve jump-to-jump labels. (maybe also check for 'jmp' here?)
          while (code[m_ip++] == Bytecode::jmp_true) {
            m_ip = read16BitOperand();
          }

          // figure out what this was for
          if (code[m_ip] == Bytecode::jmp_false) {
            m_ip += 3;
            m_operand_stack.pop_back();
          }
        } else {
          // TOS was false, don't jump. Just skip the 16 bit operand.
          m_ip += 2;
          m_operand_stack.pop_back();
        }
        break;
      case Bytecode::jmp_false:
        if (!m_operand_stack.peek().asBoolean()) {
          // TOS was false, make the jump.
          m_ip = read16BitOperand();

          // Resolve jump-to-jump labels. (maybe also check for 'jmp' here?)
          while (code[m_ip++] == Bytecode::jmp_false) {
            m_ip = read16BitOperand();
          }

          // skip past jmp_true AND its operand, which is 5 bytes.
          if (code[m_ip] == Bytecode::jmp_true) {
            m_ip += 3;
            m_operand_stack.pop_back();
          }
        } else {
          // TOS was true, don't jump. Just skip the 16 bit operand.
          m_ip += 2;
          m_operand_stack.pop_back();
        }
        break;
      case Bytecode::constant:
        m_operand_stack.push(getConstant());
        break;
      case Bytecode::new_obj:
        break;
      case Bytecode::new_array: {
        int size = read16BitOperand();
        auto arr = std::make_shared<Array>();
        // 'i' will be int when values can contain integers.
        for (double i = size; i < size; i++) {
          arr->insert(Value(i), m_operand_stack.pop());
        }
        m_operand_stack.push(Value(arr));
        break;
      }
      case Bytecode::TRUE:
        m_operand_stack.push(Value(true));
        break;
      case Bytecode::FALSE:
        m_operand_stack.push(Value(false));
        break;
      case Bytecode::null:
        m_operand_stack.push(Value(ValueType::nNoll));
        break;
      case Bytecode::gload:
        m_operand_stack.push(m_globals[read16BitOperand()]);
        break;
      case Bytecode::gstore:
        m_globals[read16BitOperand()] = m_operand_stack.peek();
        // m_operand_stack.pop_back();
        break;
      case Bytecode::load:
        m_operand_stack.push(getLocal(read16BitOperand()));
        break;
      case Bytecode::store:
        *getLocal(read16BitOperand()) = m_operand_stack.pop();
        break;
      case Bytecode::cload:
        m_operand_stack.push(*getCapturedVariable(read16BitOperand()));
        break;
      case Bytecode::cstore:
        *getCapturedVariable(read16BitOperand()) = m_operand_stack.pop();
        break;
      case Bytecode::aload:
      case Bytecode::astore: {
        Value key = m_operand_stack.pop();
        Value _arr = m_operand_stack.pop();
        if (!_arr.isArray())
          runtimeError("Attempted array access [expr] was not an array.");
        Array& arr = _arr.valueTo<Array>();
        if (op == Bytecode::aload)
          m_operand_stack.push(arr.get(key));
        else
          arr.insert(key, m_operand_stack.pop());
        break;
      }
      case Bytecode::print:
        std::cout << m_operand_stack.pop();
        break;
      case Bytecode::ret:
        m_call_stack.pop_back();
        break;
      case Bytecode::call:
        UNREACHABLE();
        break;
      case Bytecode::call_tos: {
        Value closure = m_operand_stack.pop();
        if (!closure.isClosure())
          runtimeError("Attempted invoking non-callable object.");
        call(closure);
        break;
      }
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
            auto& cv_tos = m_call_stack.peek().closure->getCapturedVariables();
            closure->addCapturedVariable(cv_tos[compiler_captured->index]);
          }
        }
        // The captured variables are now pointing to the right place, push
        // the closure to the operand stack.
        m_operand_stack.push(Value(closure));
      } break;
      case Bytecode::halt:
        break;
      default:
        UNREACHABLE();
    }
    return VMEndingStatus::VM_SUCCESS;
  }
}

}  // namespace Linaro
