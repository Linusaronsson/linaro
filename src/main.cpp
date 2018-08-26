#include <cassert>
#include <ctime>
#include <iostream>

#include "compiler/chunk.h"
#include "compiler/compiler.h"
#include "vm/value.h"
#include "linaro_utils/common.h"

#define DEBUG_VM
using namespace linaro;

int main() {
  //  uint64_t t1 = 0;
  clock_t begin = clock();
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
