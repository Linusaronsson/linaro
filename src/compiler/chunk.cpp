#include "chunk.h"

#include <cassert>
#include <iostream>

#include "../linaro_utils/common.h"
#include "compiler.h"

namespace linaro {

void BytecodeChunk::disassembleChunk() const {
  std::cout << "Bytecode disassemble:\n";
  for (unsigned i = 0; i < m_code.size();) {
    printf("%03d:   ", i);
    disassembleBytecode(static_cast<Bytecode>(m_code[i]), &i);
  }
}

void BytecodeChunk::disassembleBytecode(Bytecode op, unsigned* i) const {
  CHECK(op < Bytecode::NUM_BYTECODES);
  printf("%s", bytecodeToString(op));
  (*i)++;
  switch (getNumArguments(op)) {
    case 0:
      break;
    case 1:
      printf("   %d", read16Bits(*i));
      *i += 2;
      break;
    case 2:
      printf("   %d   %d", read16Bits(*i), read16Bits(*i + 2));
      *i += 4;
      break;
    default:
      UNREACHABLE();
  }
  std::cout << '\n';
}

const char* BytecodeChunk::bytecodeToString(Bytecode op) const {
  CHECK(op < Bytecode::NUM_BYTECODES);
  return bytecode_to_string[(uint8_t)op];
}

int BytecodeChunk::getNumArguments(Bytecode op) const {
  CHECK(op < Bytecode::NUM_BYTECODES);
  switch (op) {
    case Bytecode::jmp:
    case Bytecode::jmp_true:
    case Bytecode::jmp_false:
    case Bytecode::constant:
    case Bytecode::new_obj:
    case Bytecode::gload:
    case Bytecode::gstore:
    case Bytecode::call:
    case Bytecode::closure:
    case Bytecode::load:
    case Bytecode::store:
    case Bytecode::cload:
    case Bytecode::cstore:
      return 1;
      return 2;
    default:
      return 0;
  }
}

}  // namespace linaro