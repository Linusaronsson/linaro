#ifndef CHUNK_H
#define CHUNK_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace linaro {

struct Location;

#define BYTECODE(name) name,
enum Bytecode : uint8_t {
#include "bytecodes.h"
  NUM_BYTECODES
};
#undef BYTECODE

#define BYTECODE(name) #name,
const char* const bytecode_to_string[Bytecode::NUM_BYTECODES]{
#include "bytecodes.h"
};
#undef BYTECODE

class BytecodeChunk {
 public:
  BytecodeChunk() {}
  const std::vector<uint8_t>& code() { return m_code; }
  const Location& getLocation(int i) { return *m_loc[i]; }
  size_t chunkSize() const { return m_code.size(); }
  size_t currentOffset() const { return m_code.size() + 1; }

  // Extracting data from chunk
  inline uint8_t readByte(int i) const { return m_code[i]; }
  inline uint16_t read16Bits(int i) const {
    return readByte(i) | (readByte(i + 1) << 8);
  }

  inline uint32_t read32Bits(int i) const {
    return read16Bits(i) | (read16Bits(i + 2) << 16);
  }

  // Adding data to chunk
  inline void addByte(uint8_t op) { m_code.push_back(op); }
  inline void add16Bits(uint16_t arg) {
    addByte((uint8_t)arg);
    addByte((uint8_t)(arg >> 8));
  }

  inline void add32Bits(uint32_t arg) {
    add16Bits((uint16_t)arg);
    add16Bits((uint16_t)(arg >> 16));
  }

  // Debug
  void disassembleChunk() const;

 private:
  inline void disassembleBytecode(Bytecode op, unsigned* i) const;
  inline const char* bytecodeToString(Bytecode op) const;
  int getNumArguments(Bytecode op) const;

  std::vector<uint8_t> m_code;  // the code
  // Each bytecode is associated with a location in the source file.
  // This is used for reporting potential errors at runtime.
  std::vector<Location*> m_loc;
};

}  // Namespace linaro

#endif  // CHUNK_H