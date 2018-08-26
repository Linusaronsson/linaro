#include "utils.h"

namespace linaro {
std::string readFile(const char *filename) {
  std::ifstream s(filename);
  std::string src;
  char c;
  if (s.is_open()) {
    while (s.get(c)) src.push_back(c);
    s.close();
  } else {
    printf("Unable to open file: %s\n", filename);
    return nullptr;
  }

  return src;
}
}  // namespace linaro
