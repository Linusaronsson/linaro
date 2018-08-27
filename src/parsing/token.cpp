#include "token.h"

namespace linaro {

std::string_view Token::asString() const {
  switch (m_type) {
    case Token::SYMBOL:
    case Token::STRING:
    case Token::NUMBER:
    case Token::UNKNOWN:
      return m_str;
    default:
      return std::string_view{tokenString(m_type)};
  }
}

std::ostream& operator<<(std::ostream& cout, const Token& tok) {
  cout << tok.m_location << " Token(" << tok.tokenName(tok.type()) << ", "
       << tok.asString() << ")";
  return cout;
}
}  // namespace linaro