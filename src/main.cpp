#include <iostream>

#include "compiler/chunk.h"

using namespace Linaro;

int main() {
	try {
		BytecodeChunk* chunk = new BytecodeChunk();
		chunk->disassembleChunk();
	} catch(std::runtime_error e) {
		std::cout << e.what() << '\n';
	}
	return 0;
}
