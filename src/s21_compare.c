#include "s21_decimal.h"

double s21_convert_to_double(s21_decimal value, int i) {
  int exp = s21_get_exp(value);
  return (double)((unsigned int)value.bits[i] / pow(10, exp));
}

// Функция для сравнения чисел без учета знака
int s21_compare_magnitude(s21_decimal value_1, s21_decimal value_2) {
  int result = 0;

  for (int i = 2; i >= 0; i--) {
    double num_1 = s21_convert_to_double(value_1, i);
    double num_2 = s21_convert_to_double(value_2, i);

    if (num_1 > num_2) {
      result = 1;
      break;
    } else if (num_2 > num_1) {
      result = -1;
      break;
    }
  }
  return result;
}

// Функция для сравнения двух чисел
int s21_compare(s21_decimal value_1, s21_decimal value_2) {
  int sign_1 = s21_get_sign(value_1);
  int sign_2 = s21_get_sign(value_2);

  if (sign_1 != sign_2) {
    return sign_1 == 0 ? 1 : -1;
  }
  int magnitudeComparison = s21_compare_magnitude(value_1, value_2);
  return sign_1 == 0 ? magnitudeComparison : -magnitudeComparison;
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int result = 1;
  if (!s21_is_zero(value_1) || !s21_is_zero(value_2)) {
    for (int i = 0; i < 4; i++) {
      if (value_1.bits[i] != value_2.bits[i]) {
        result = 0;
        break;
      }
    }
  }
  return result;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_equal(value_1, value_2);
}

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  return s21_compare(value_1, value_2) == 1;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return !s21_is_greater(value_1, value_2) &&
         s21_is_not_equal(value_1, value_2);
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_greater(value_1, value_2) || s21_is_equal(value_1, value_2);
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return s21_is_less(value_1, value_2) || s21_is_equal(value_1, value_2);
}
