#include "token.h"

namespace linaro {

std::ostream& operator<<(std::ostream& cout, const Token& tok) {
  cout << tok.m_location << " Token(" << tok.tokenString(tok.type()) << ", "
       << tok.m_str << ")";
  return cout;
}
}  // namespace linaro