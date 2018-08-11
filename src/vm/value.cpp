#include "value.h"

namespace linaro {

void Number::print() const override {
    std::cout << m_val;
}
    
size_t Number::hash() const override {
    return std::hash(m_val);
}

} // Namespace 

