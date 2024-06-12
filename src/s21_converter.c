#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int result = 0;
  if (!dst)
    result = 1;
  else {
    s21_set_zero(dst);
    s21_set_sign(dst, src < 0);
    dst->bits[0] = abs(src);
  }

  return result;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int code = 0;
  if (!dst) {
    code = 1;  // Ошибка: нулевой указатель
  } else {
    s21_decimal result;
    int truncateResult = s21_truncate(src, &result);

    int isOverflowed =
        (result.bits[2] != 0) || (result.bits[1] != 0) ||
        ((result.bits[0] > 2147483647) && !s21_get_sign(result)) ||
        ((result.bits[0] > 2147483648) && s21_get_sign(result));

    if (truncateResult || isOverflowed) {
      code = 1;  // Ошибка конвертации
    } else {
      int sign = s21_get_sign(result) ? -1 : 1;
      *dst = result.bits[0] * sign;
    }
  }

  return code;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int result = 0;
  if (!dst) {
    result = 1;  // Ошибка: нулевой указатель
  } else {
    *dst = src.bits[2] * pow(2, 64) + src.bits[1] * pow(2, 32) + src.bits[0];
    *dst /= pow(10, s21_get_exp(src));
    if (s21_get_sign(src)) *dst *= -1;
  }
  return result;
}

s21_decimal s21_shift_decimal_left(s21_decimal x, unsigned int shift) {
  for (unsigned int i = 0; i < shift; i++) {
    for (int bit = 2; bit > 0; bit--) {
      x.bits[bit] <<= 1;
      x.bits[bit] |= (x.bits[bit - 1] & (1U << 31)) >> 31;
    }
    x.bits[0] <<= 1;
  }
  return x;
}

s21_decimal s21_shift_decimal_right(s21_decimal x, unsigned int shift) {
  for (unsigned int i = 0; i < shift; i++) {
    for (int bit = 0; bit < 2; bit++) {
      x.bits[bit] >>= 1;
      x.bits[bit] |= (x.bits[bit + 1] & 1) << 31;
    }
    x.bits[2] >>= 1;
  }
  return x;
}

int s21_mul_decimal_by_10(s21_decimal value, s21_decimal *result) {
  int error = 0;

  s21_decimal shift3 = s21_shift_decimal_left(value, 3);
  s21_decimal shift1 = s21_shift_decimal_left(value, 1);

  error = s21_is_not_equal(shift3, s21_shift_decimal_right(shift1, 3));
  error = s21_add_decimal_mantissa(shift3, shift1, result);

  s21_set_exp(result, s21_get_exp(value) + 1);
  error = (s21_get_exp(*result) > 28);

  return error;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int error = 0;
  s21_set_zero(dst);

  error = (0 < fabs(src) && fabs(src) < 1e-28) ||
          (fabs(src) > 79228162514264333195497439231.0) || isinf(src) ||
          isnan(src);

  char str[15];
  memset(str, '\0', sizeof(str));

  sprintf(str, "%.6e", src);

  int exp = 0;
  int pt1 = 0, pt2 = 0;

  sscanf(str, "%d.%de%d", &pt1, &pt2, &exp);

  if (pt1 < 0) pt1 *= -1;

  dst->bits[0] = pt1 * pow(10, 6) + pt2;
  exp -= 6;

  for (int i = 0; i < exp; i++) {
    s21_mul_decimal_by_10(*dst, dst);
  }

  s21_set_exp(dst, -exp * (exp < 0));

  while (s21_get_exp(*dst) > 28) {
    *dst = s21_div_by_ten(*dst);
    s21_set_exp(dst, s21_get_exp(*dst) - 1);
  }

  if (s21_get_exp(*dst) == 28) {
    float f = 0;
    s21_from_decimal_to_float(*dst, &f);

    if (src - f > 5 / pow(10, s21_get_exp(*dst) + 1)) {
      s21_add_decimal_mantissa(*dst, ((s21_decimal){{1, 0, 0, 0}}), dst);
    }
  }

  *dst = s21_normalize_decimal(*dst);
  s21_set_sign(dst, src < 0);

  return error;
}
