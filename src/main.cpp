#include <cassert>
#include <ctime>
#include <iostream>

#include "compiler/chunk.h"
#include "compiler/compiler.h"
#include "linaro_utils/logging.h"
#include "vm/constant_pool.h"
#include "vm/value.h"
#include "vm/vm.h"

#define DEBUG_VM

using namespace linaro;

int main() {
  //  uint64_t t1 = 0;
  clock_t begin = clock();
  std::cout << "Entered main..." << std::endl;
  linaro::ConstantPool p;
  for(int i = 0; i < 100000; i++) {
    Value v = std::to_string(i).c_str();
    p.addIfNew(v);
  }
  //p.printConstantPool();
  std::cout << "\n\n\n";

#ifdef DEBUG_LEXER
  // Lexer debug code here
#endif

#ifdef DEBUG_PARSER
  // Parser debug code here
#endif

#ifdef DEBUG_CODE_GENERATOR
  // Code generation debug code here
#endif

#ifdef DEBUG_VM
  // VM debug code here
#endif

  clock_t end = clock();
  std::cout << "Execution time: " << double(end - begin) / CLOCKS_PER_SEC
            << "\n";
  return 0;
}
