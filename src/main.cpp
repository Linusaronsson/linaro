#include <string.h>
#include <ctime>
#include <iostream>
#include <string_view>

#include "ast/ast.h"
#include "ast/expression.h"
#include "ast/statement.h"
#include "code_generator/chunk.h"
#include "code_generator/code_generator.h"
#include "linaro_utils/common.h"
#include "linaro_utils/utils.h"
#include "parsing/lexer.h"
#include "parsing/token.h"
#include "vm/value.h"

#define DEBUG_CODE_GENERATOR
using namespace Linaro;

int main() {
  //  uint64_t t1 = 0;
  clock_t begin = clock();
#ifdef DEBUG_LEXER
  Lexer lex("script.lo");
  Token t = lex.nextToken();
  while (t.type() != TokenType::END) {
    std::cout << t << std::endl;
    t = lex.nextToken();
  }
  // Lexer debug code here
#endif

#ifdef DEBUG_PARSER
  // Parser debug code here
  Parser parser("script.lo");
  auto AST = parser.parse();

  AST->printNode();
#endif

#ifdef DEBUG_CODE_GENERATOR
  // Code generation debug code here
  Parser parser("script.lo");
  auto AST = parser.parse();
  auto fn = CodeGenerator::compile(AST.get());
#ifdef DEBUG
  auto functions = CodeGenerator::getFunctions();
  std::cout << "\n---- PROGRAM DEBUG ----\n\n";
  for (const auto& fn : functions) {
    fn->printFunction();
  }
#endif
#endif

#ifdef DEBUG_VM
  // VM debug code here
#endif

  clock_t end = clock();
  std::cout << "Execution time: " << double(end - begin) / CLOCKS_PER_SEC
            << "\n";
  return 0;
}
