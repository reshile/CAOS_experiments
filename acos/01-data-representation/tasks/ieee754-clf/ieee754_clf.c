#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "ieee754_clf.h"

float_class_t classify(double x) {
  bool is_neg = false;
  uint64_t n = 0;
  memcpy(&n, &x, sizeof x);
  uint64_t mask = 1;
  mask <<= 63;
  if ((n & mask) == 0x8000000000000000) {
    is_neg = true;
  }

  uint64_t exp_mask = 0x7FF0000000000000;
  uint64_t exp = n & exp_mask;

  uint64_t matiss_mask = 0x000FFFFFFFFFFFFF;
  uint64_t mantiss = n & matiss_mask;

  if ((exp == 0) && (mantiss == 0)) {
    if (is_neg) {
      return MinusZero;
    }
    return Zero;
  }

  if ((exp == 0) && (mantiss != 0)) {
    if (is_neg) {
      return MinusDenormal;
    }
    return Denormal;
  }

  if ((exp == exp_mask) && (mantiss == 0)) {
    if (is_neg) {
      return MinusInf;
    }
    return Inf;
  }

  if ((exp == exp_mask) && (mantiss != 0)) {
    return NaN;
  }

  if ((exp != exp_mask) && (exp != 0)) {
    if (is_neg) {
      return MinusRegular;
    }
    return Regular;
  }

  return NaN;
}
