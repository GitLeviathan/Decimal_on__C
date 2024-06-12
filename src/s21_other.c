#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  int res = 0;
  if (!result) {
    res = 1;  // Ошибка: нулевой указатель
  } else {
    // XOR
    value.bits[3] ^= 0x80000000;  // 1
    *result = value;
  }
  return res;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int res = 0;
  if (!result) {
    res = 1;  // Ошибка: нулевой указатель
  } else if (s21_is_zero(value)) {
    s21_set_zero(result);
  } else {
    int exp = s21_get_exp(value);
    s21_decimal temp = value;
    // Делим на 10 в степени exp, отнимая от bits[3] в каждом шаге
    for (; exp > 0; exp--) {
      temp = s21_div_by_ten(temp);
      temp.bits[3] -= 65536;
    }
    if (temp.bits[0] == 0) temp.bits[3] = 0;
    *result = temp;
  }
  return res;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int res = 0;
  if (!result) {
    res = 1;  // Ошибка: нулевой указатель
  } else {
    s21_decimal trunc;
    s21_set_zero(&trunc);
    s21_set_zero(result);
    value = s21_normalize_decimal(value);
    s21_truncate(value, &trunc);
    *result = trunc;
    s21_set_sign(result, s21_get_sign(value));
    if (!s21_is_equal(value, trunc) && s21_get_sign(value)) {
      s21_add_decimal_mantissa(*result, (s21_decimal){{1, 0, 0, 0}}, result);
    }
  }
  return res;
}

// delete returns
int s21_round(s21_decimal value, s21_decimal *result) {
  if (!result) {
    return 1;  // Ошибка: нулевой указатель
  }

  value = s21_normalize_decimal(value);
  s21_decimal truncated;
  s21_truncate(value, &truncated);

  int exp = s21_get_exp(value);
  int sign = s21_get_sign(value);

  if (s21_is_zero(truncated) || exp == 0) {
    // Если значение уже целое, просто устанавливаем его и возвращаем 0
    *result = truncated;
    return 0;
  }

  for (int exptemp = exp; exptemp > 1; exptemp--) {
    value = s21_div_by_ten(value);
    value.bits[3] -= 65536;
  }

  int remainder = s21_mod_ten_remainder(value);
  int isRoundUp =
      remainder >= 5 &&
      ((exp == 1 && (remainder > 5 || truncated.bits[0] % 2 == 1)) || exp > 1);

  if (isRoundUp) {
    s21_add_decimal_mantissa(truncated, (s21_decimal){{1, 0, 0, 0}}, result);
  } else {
    *result = truncated;
  }

  s21_set_sign(result, sign);
  return 0;
}