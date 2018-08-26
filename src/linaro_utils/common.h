#ifndef COMMON_H
#define COMMON_H

namespace linaro {

// Constants
const int MAX_PARAMETERS = 8;
const int MAX_SYMBOL_NAME = 32;
const int MAX_SYMBOL_PER_SCOPE = 64;

// Debug

#ifdef DEBUG

#include <stdio.h>

// Logging (only during development, i.e when DEBUG is defined)

// Assert
#define CHECK(condition)                                                    \
  do {                                                                      \
    if (!(condition)) {                                                     \
      fprintf(stderr, "%s: %d Assert failed in %s()\n", __FILE__, __LINE__, \
              __func__);                                                    \
      exit(1);                                                              \
    }                                                                       \
  } while (0)

// Reports error when unreachable code is reached
#define UNREACHABLE()                                                         \
  do {                                                                        \
    fprintf(stderr, "%s: %d: Unreachable code in %s()\n", __FILE__, __LINE__, \
            __func__);                                                        \
    exit(1);                                                                  \
  } while (0)

#else

#define CHECK(condition) \
  do {                   \
  } while (0)

#define UNREACHABLE()
#endif

}  // namespace linaro

#endif  // COMMON_H