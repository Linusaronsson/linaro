#include "vm.h"
#include "../compiler/compiler.h"
#include "value.h"

namespace linaro {

VMEndingStatus VM::interpret(const std::string& source, const char* filename) {
  // initialize VM (todo)

  // run the code
  VMEndingStatus res = execute(m_main_code);

  // turn off vm (todo)
  delete m_globals;

  // return status code
  return res;
}

uint32_t VM::read32bitOperand(uint8_t* ip) {
  return ip[0] | (ip[1] << 8) | (ip[2] << 16) | (ip[3] << 24);
}

uint16_t VM::read16bitOperand(bytecode* ip) {}

uint8_t VM::read8bitOperand(bytecode* ip) {}

void VM::binaryoperation(bytecode type) {}

VMEndingStatus VM::execute(BytecodeChunk* code) {
  // std::vector<uint8_t>& c = *code->Code();
  // uint32_t ip = 0;
  for (;;) {
    // Bytecode op = static_cast<Bytecode>(c[ip++]);
    // switch (op) {

    // }
  }
  return VMEndingStatus::VM_SUCCESS;
}
}  // Namespace linaro
