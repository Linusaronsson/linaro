#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <cstddef>
#include <vector>

namespace Linaro {
								 
#define BYTECODE(name) name, 
enum class Bytecode : uint8_t { 
    #include "bytecodes.h"
    NUM_BYTECODES 
};
#undef BYTECODE

#define BYTECODE(name) #name,
const char* const 
bytecode_to_string[(long)Bytecode::NUM_BYTECODES] {
    #include "bytecodes.h"
};
#undef BYTECODE

class BytecodeChunk {
public:
    BytecodeChunk() {}
    std::vector<uint8_t>* code() { return &m_code; }
    size_t chunk_size() const { return m_code.size(); }
	size_t current_offset() const { return m_code.size() + 1; }
    void push_back(uint8_t op) { m_code.push_back(op); }
    void disassembleChunk() const;
private:
    int getNumArguments(uint8_t op) const;
    void disassembleBytecode(uint8_t op) const;
    const char* bytecodeToString(uint8_t op) const;

    std::vector<uint8_t> m_code;
};

} // Namespace linaro

#endif // CHUNK_H