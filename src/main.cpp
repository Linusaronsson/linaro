#include <iostream>
#include <ctime>

#include "compiler/chunk.h"
#include "linaro_utils/logging.h"
#include "compiler/compiler.h"
#include "vm/vm.h"

using namespace linaro;

int main() {
	uint64_t t1 = 0;
	clock_t begin = clock();

#ifdef DEBUG_LEXER
	//Lexer debug code here
#endif

#ifdef DEBUG_PARSER
	//Parser debug code here
#endif


#ifdef DEBUG_CODE_GENERATOR
	//Code generation debug code here
#endif

#ifdef DEBUG_VM
	//VM debug code here
#endif
	
	clock_t end = clock();
	std::cout << "Execution time: " << double(end - begin) / CLOCKS_PER_SEC << "\n";

}

