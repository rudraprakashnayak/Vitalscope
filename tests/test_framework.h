#pragma once

#include <cmath>
#include <iostream>

namespace testing {
inline int& failures() {
    static int f = 0;
    return f;
}
}  // namespace testing

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            ++testing::failures();                                               \
            std::cout << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #cond "\n"; \
        }                                                                        \
    } while (0)

#define CHECK_CLOSE(a, b)                                                        \
    do {                                                                         \
        double da = (a);                                                         \
        double db = (b);                                                         \
        if (std::fabs(da - db) > 1e-6) {                                         \
            ++testing::failures();                                               \
            std::cout << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #a       \
                      << " ~= " #b << " (" << da << " vs " << db << ")\n";       \
        }                                                                        \
    } while (0)
