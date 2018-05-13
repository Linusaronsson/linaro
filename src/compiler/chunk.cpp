#include "chunk.h"

#include <cassert>

#include "compiler.h"
#include "../linaro_utils/logging.h"

namespace Linaro {

void BytecodeChunk::disassembleChunk() const {
	getNumArguments(0);
}

void BytecodeChunk::disassembleBytecode(uint8_t op) const {
	//printf("%s\n", )
}

const char* BytecodeChunk::bytecodeToString(uint8_t op) const {
	assert(op < (uint8_t)Bytecode::NUM_BYTECODES);
	return bytecode_to_string[op];
}

int BytecodeChunk::getNumArguments(uint8_t op) const {
	switch (op) {
	default:
		UNREACHABLE();
	}
}

} // Namespace Linaro