#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <cstddef>
#include <vector>

#include "../parsing/token.h"

namespace linaro {

using bytecode = uint8_t;
								 
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
    std::vector<bytecode>* code() { return &m_code; }
    size_t chunk_size() const { return m_code.size(); }
	size_t current_offset() const { return m_code.size() + 1; }
    void push_back(bytecode op) { m_code.push_back(op); }
    void disassembleChunk() const;
private:
    int getNumArguments(bytecode op) const;
    void disassembleBytecode(bytecode op) const;
    const char* bytecodeToString(bytecode op) const;

    std::vector<bytecode> m_code; // the code
    // Each bytecode is associated with a location in the source file.
    // This is used for reporting potential errors at runtime.
    std::vector<Location> m_loc; 
};

} // Namespace linaro

#endif // CHUNK_H