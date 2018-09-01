#ifndef VM_INSTANCE_H
#define VM_INSTANCE_H

#include "../code_generator/chunk.h"
#include "value.h"

namespace Linaro {

/*
 * An instance that can be run by the VM.
 * This class can be seralized and saved in
 * a file to be executed at a later time.
 * Similar to how a .class file works.
 */
class VMContext {
 public:
  VMContext(int num_globals, const BytecodeChunk& main_code)
      : m_num_globals(num_globals), m_main_code(main_code) {}

  ~VMContext() {}

  // save VM instance in file
  void serialize(const char* filename);

  const BytecodeChunk& main_code() { return m_main_code; }
  int global_space() { return m_num_globals; }

 private:
  int m_num_globals;
  BytecodeChunk m_main_code;
};

/**
 * Comment on VMContext: a VM context will intially be allocated in
 * the heap by the codegenerator and fill it with the necessary
 * information to start a VM and execute the program.
 * */

}  // namespace Linaro

#endif  // VM_INSTANCE_H
