#include "chunk.h"

#include <cassert>

#include "compiler.h"
#include "../linaro_utils/logging.h"

namespace linaro {

void BytecodeChunk::disassembleChunk() const {
	getNumArguments(0);
}

void BytecodeChunk::disassembleBytecode(bytecode op) const {
	//printf("%s\n", )
}

const char* BytecodeChunk::bytecodeToString(bytecode op) const {
	assert(op < (bytecode)Bytecode::NUM_BYTECODES);
	return bytecode_to_string[op];
}

int BytecodeChunk::getNumArguments(bytecode op) const {
	switch (op) {
	default:
		UNREACHABLE();
	}
}

} // Namespace Linaro