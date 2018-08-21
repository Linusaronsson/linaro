#ifndef VM_INSTANCE_H
#define VM_INSTANCE_H

#include "../compiler/chunk.h"
#include "constant_pool.h"
#include "value.h"

namespace linaro {

/*
 * An instance that can be run by the VM.
 * This class can be seralized and saved in
 * a file to be executed at a later time.
 * Similar to how a .class file works.
 */
class VMContext {
 public:
  VMContext(int num_globals, const ConstantPool& const_pool,
            const BytecodeChunk& main_code)
      : m_num_globals(num_globals),
        m_const_pool(const_pool),
        m_main_code(main_code) {}

  ~VMContext() {}

  // save VM instance in file
  void serialize(const char* filename);

  const BytecodeChunk& main_code() { return m_main_code; }
  const ConstantPool& const_pool() { return m_const_pool; }
  int global_space() { return m_num_globals; }

 private:
  int m_num_globals;
  ConstantPool m_const_pool;
  BytecodeChunk m_main_code;
};

/**
 * Comment on VMContext: a VM context will intially be allocated in
 * the heap by the codegenerator and fill it with the necessary
 * information to start a VM and execute the program.
 * */

}  // Namespace linaro

#endif  // VM_INSTANCE_H
