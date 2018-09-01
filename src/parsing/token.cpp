#include "token.h"

namespace Linaro {

std::string_view Token::asString() const {
  switch (m_type) {
    case TokenType::SYMBOL:
    case TokenType::STRING:
    case TokenType::NUMBER:
    case TokenType::UNKNOWN:
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
}  // namespace Linaro