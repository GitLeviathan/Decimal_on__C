#include "s21_decimal.h"

int s21_is_big_ok(big_decimal num) {
  // проверяет что биты в БигДец старше 3го - пустые
  int res = 1;
  for (int i = 6; i >= 3 && res; i--) {
    if (num.bits[i]) res = 0;
  }
  return res;
}

int s21_get_overflow(big_decimal *num) {  // проверяет переполнение БигДец
  uint64_t overflow = 0;
  int flag = 0;
  for (int i = 0; i < 7; i++) {
    num->bits[i] += overflow;
    num->bits[i] &= MAXBITS;
  }
  if (!s21_is_big_ok(*num)) flag = 1;

  return flag;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  // проверяем что степени меньше 28, при необходимости снижаем до 28
  if (s21_get_exp(value_1) > 28 || s21_get_exp(value_2) > 28) {
    int diffExpA = s21_get_exp(value_1) - 28;
    int diffExpB = s21_get_exp(value_2) - 28;
    s21_down_exp(value_1, &value_1, diffExpA);
    s21_down_exp(value_2, &value_2, diffExpB);
  }
  if (s21_is_zero(value_1) && s21_is_zero(value_1)) {
    s21_set_zero(result);
  } else
  // приводим все к одной степени (в случае переполнения при повышении степени
  // одного числа снижаем степень второго)
  {
    value_1 = s21_normalize_decimal(value_1);
    value_2 = s21_normalize_decimal(value_2);
    s21_equalize_exp(&value_1, &value_2);
    int signA = s21_get_sign(value_1);
    int signB = s21_get_sign(value_2);
    int flag = 1;
    int cout = 0;
    big_decimal aA = {0};
    big_decimal bB = {0};
    big_decimal rRes = {0};
    // если знаки одинаковые, то складываем значения
    if (signA == signB) {
      s21_dec_to_big(value_1, &aA);
      s21_dec_to_big(value_2, &bB);
      rRes = s21_sum_mantissa_big(aA, bB);
      flag = s21_get_overflow(&rRes);
      if (flag && aA.scale > 0) {
        for (cout = 0; flag > 0; cout++) {
          s21_down_exp_big(rRes, &rRes, 1);
          flag = s21_get_overflow(&rRes);
        }
      }
      // если есть переполнение по сумме и степень 0 то число слишком большое
      else if (flag && aA.scale == 0) {
        if (signA == 1) {
          return ret = 2;
        } else {
          return ret = 1;
        }
      }
      s21_big_to_dec(rRes, result);
      result->bits[3] = value_1.bits[3] - (65536 * cout);
      *result = s21_normalize_decimal(*result);
    }  // если знаки разные - сравниваем мантисы
    else {
      int compare = s21_compare_magnitude(value_1, value_2);
      // если первая мантиса больше, то отнимаем от первой вторую и знак
      // оставляем от первой
      if (compare == 1) {
        s21_sub_mantissa(value_1, value_2, result);
        result->bits[3] = value_1.bits[3];
      }
      // если вторая мантисса больше, то отнимаем  от второй первую и знак берем
      // от второй
      else if (compare == -1) {
        s21_sub_mantissa(value_2, value_1, result);
        result->bits[3] = value_2.bits[3];
      }
    }
  }
  return ret;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  // проверяем что степени меньше 28, при необходимости снижаем до 28
  if (s21_get_exp(value_1) > 28 || s21_get_exp(value_2) > 28) {
    int diffExpA = s21_get_exp(value_1) - 28;
    int diffExpB = s21_get_exp(value_2) - 28;
    s21_down_exp(value_1, &value_1, diffExpA);
    s21_down_exp(value_2, &value_2, diffExpB);
  }
  if (s21_is_zero(value_1) && s21_is_zero(value_1)) {
    s21_set_zero(result);
  } else {
    value_1 = s21_normalize_decimal(value_1);
    value_2 = s21_normalize_decimal(value_2);
    // приводим все к одной степени (в случае переполнения при повышении степени
    // одного числа снижаем степень второго)
    s21_equalize_exp(&value_1, &value_2);
    int signA = s21_get_sign(value_1);
    int signB = s21_get_sign(value_2);
    int flag = 1;
    int cout = 0;
    big_decimal aA = {0};
    big_decimal bB = {0};
    big_decimal rRes = {0};
    // если знаки разные, то складываем значения
    if (signA != signB) {
      s21_dec_to_big(value_1, &aA);
      s21_dec_to_big(value_2, &bB);
      rRes = s21_sum_mantissa_big(aA, bB);
      flag = s21_get_overflow(&rRes);
      if (flag && aA.scale > 0) {
        for (cout = 0; flag > 0; cout++) {
          s21_down_exp_big(rRes, &rRes, 1);
          flag = s21_get_overflow(&rRes);
        }
      }
      // если есть переполнение по сумме и степень 0 то число слишком маленькое
      else if (flag && aA.scale == 0) {
        if (signA == 1) {
          return ret = 2;
        } else {
          return ret = 1;
        }
      }
      s21_big_to_dec(rRes, result);
      result->bits[3] = value_1.bits[3] - (65536 * cout);
      *result = s21_normalize_decimal(*result);
    }
    // если знаки одинаковые - сравниваем мантисы
    else {
      int compare = s21_compare_magnitude(value_1, value_2);
      // если первая мантиса больше, то отнимаем от первой вторую и знак
      // оставляем от первой
      if (compare == 1) {
        s21_sub_mantissa(value_1, value_2, result);
        result->bits[3] = value_2.bits[3];
      }
      // если вторая мантисса больше, то отнимаем от второй первую и знак ставим
      // отрицательный
      else if (compare == -1) {
        s21_sub_mantissa(value_2, value_1, result);
        result->bits[3] = value_2.bits[3];
        s21_set_sign(result, (!signB));
      } else if (compare == 0) {
        s21_set_zero(result);
      }
      *result = s21_normalize_decimal(*result);
    }
  }
  return ret;
}

#define NULL_big_decimal       \
  (s21_big_decimal) {          \
    { 0, 0, 0, 0, 0, 0, 0, 0 } \
  }

int s21_mul(s21_decimal valueL, s21_decimal valueR, s21_decimal *result) {
  s21_big_decimal tmpResult = NULL_big_decimal;
  int error = 0;

  s21_set_zero(result);

  for (int bit = 0; bit < 3; bit++) {
    for (int i = 31; i >= 0; i--) {
      if ((valueR.bits[bit] & (1U << i)) != 0) {
        s21_big_decimal tmp = s21_shift_big_decimal_left(
            s21_from_decimal_to_big_decimal(valueL), 32 * bit + i);
        s21_add_big_decimal_mantissa(tmpResult, tmp, &tmpResult);
      }
    }
  }

  int sign = (s21_get_sign(valueL) + s21_get_sign(valueR)) % 2;
  s21_set_big_decimal_sign(&tmpResult, sign);

  int res_exp = s21_get_exp(valueL) + s21_get_exp(valueR);
  s21_set_big_decimal_exponent(&tmpResult, res_exp);

  *result = s21_round_big_decimal(tmpResult, &error);

  return error;
}

int s21_big_decimal_is_greater_or_equal(s21_big_decimal x, s21_big_decimal y) {
  return s21_big_decimal_is_equal(x, y) || s21_big_decimal_is_greater(x, y);
}

s21_big_decimal s21_big_decimal_or(s21_big_decimal x, s21_big_decimal y) {
  for (int i = 0; i < 7; i++) {
    x.bits[i] |= y.bits[i];
  }
  return x;
}

int s21_div(s21_decimal value1, s21_decimal value2, s21_decimal *result) {
  s21_set_zero(result);
  int error = 0;

  if (s21_is_equal(value2, (s21_decimal){{0, 0, 0, 0}})) {
    return 3;
  }

  s21_big_decimal tmp1 = s21_from_decimal_to_big_decimal(value1);
  s21_big_decimal tmp2 = s21_from_decimal_to_big_decimal(value2);
  s21_big_decimal tmpResult = NULL_big_decimal;

  int exp = s21_get_big_decimal_exponent(tmp1);
  s21_big_decimal buff = NULL_big_decimal;
  if (!s21_is_equal(value1, (s21_decimal){{0, 0, 0, 0}})) {
    while (!s21_mul_big_decimal_by_10(tmp1, &buff)) {
      tmp1 = buff;
      exp++;
    }
  }

  tmp1.bits[7] = 0;
  tmp2.bits[7] = 0;

  for (int i = 223; i >= 32; i--) {
    if (s21_big_decimal_is_greater_or_equal(
            s21_shift_big_decimal_right(tmp1, i), tmp2)) {
      tmpResult = s21_big_decimal_or(
          tmpResult, s21_shift_big_decimal_left(
                         ((s21_big_decimal){{1, 0, 0, 0, 0, 0, 0, 0}}), i));

      s21_sub_big_decimal_mantissa(tmp1, s21_shift_big_decimal_left(tmp2, i),
                                   &tmp1);
    }
  }

  int sign = (s21_get_sign(value1) + s21_get_sign(value2)) % 2;
  s21_set_big_decimal_sign(&tmpResult, sign);

  s21_set_big_decimal_exponent(&tmpResult, exp - s21_get_exp(value2));

  *result = s21_round_big_decimal(tmpResult, &error);
  return error;
}
