#ifndef GEOHEX3_POW_H
#define GEOHEX3_POW_H

#include "geohex3/macro.h"
#include <math.h>

static inline long geohex_pow3(int y) {
  if (y > 18) {
    return (long)pow(3.0, y);
  }
  else {
    // for performance :)
    static const long GEOHEX3_CALCED_POW3[] = {
      GEOHEX3_MACRO_POW(3L,  0),
      GEOHEX3_MACRO_POW(3L,  1),
      GEOHEX3_MACRO_POW(3L,  2),
      GEOHEX3_MACRO_POW(3L,  3),
      GEOHEX3_MACRO_POW(3L,  4),
      GEOHEX3_MACRO_POW(3L,  5),
      GEOHEX3_MACRO_POW(3L,  6),
      GEOHEX3_MACRO_POW(3L,  7),
      GEOHEX3_MACRO_POW(3L,  8),
      GEOHEX3_MACRO_POW(3L,  9),
      GEOHEX3_MACRO_POW(3L, 10),
      GEOHEX3_MACRO_POW(3L, 11),
      GEOHEX3_MACRO_POW(3L, 12),
      GEOHEX3_MACRO_POW(3L, 13),
      GEOHEX3_MACRO_POW(3L, 14),
      GEOHEX3_MACRO_POW(3L, 15),
      GEOHEX3_MACRO_POW(3L, 16),
      GEOHEX3_MACRO_POW(3L, 17),
      GEOHEX3_MACRO_POW(3L, 18)
    };
    return GEOHEX3_CALCED_POW3[y];
  }
}

static inline long geohex_pow10(int y) {
  if (y > 17) {
    return (long)pow(10.0, y);
  }
  else {
    // for performance :)
    static const long GEOHEX3_CALCED_POW10[] = {
      GEOHEX3_MACRO_POW(10L,  0),
      GEOHEX3_MACRO_POW(10L,  1),
      GEOHEX3_MACRO_POW(10L,  2),
      GEOHEX3_MACRO_POW(10L,  3),
      GEOHEX3_MACRO_POW(10L,  4),
      GEOHEX3_MACRO_POW(10L,  5),
      GEOHEX3_MACRO_POW(10L,  6),
      GEOHEX3_MACRO_POW(10L,  7),
      GEOHEX3_MACRO_POW(10L,  8),
      GEOHEX3_MACRO_POW(10L,  9),
      GEOHEX3_MACRO_POW(10L, 10),
      GEOHEX3_MACRO_POW(10L, 11),
      GEOHEX3_MACRO_POW(10L, 12),
      GEOHEX3_MACRO_POW(10L, 13),
      GEOHEX3_MACRO_POW(10L, 14),
      GEOHEX3_MACRO_POW(10L, 15),
      GEOHEX3_MACRO_POW(10L, 16),
      GEOHEX3_MACRO_POW(10L, 17)
    };
    return GEOHEX3_CALCED_POW10[y];
  }
}

#endif
