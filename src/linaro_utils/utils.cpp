#include "utils.h"

namespace Linaro {

std::string readFile(const char *filename) {
  std::ifstream f(filename);
  std::stringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

}  // namespace Linaro
