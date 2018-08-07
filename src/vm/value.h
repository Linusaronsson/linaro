#ifndef VALUE_H
#define VALUE_H

namespace linaro {

enum ValueType {
    NUMBER,
    STRING,
    FUNCTION,
    ARRAY,
    OBJECT,
    THREAD,
    NOLL
};

/*
 * Value represents any data type (dynamically typed).
 */
class Value {
public:
private:
    ValueType m_type;
    double m_value;
};

} // Namespace linaro

#endif // VALUE_H