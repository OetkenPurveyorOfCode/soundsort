#pragma once
#include <stdint.h>

#define shuffle(Ts, len) \
    for (size_t i = (len)-1; i >= 1; i--) { \
        uint32_t j = pcg32_boundedrand(i+1); \
        typeof(*(Ts)) T = Ts[i]; \
        Ts[i] = Ts[j]; \
        Ts[j] = T; \
    }
