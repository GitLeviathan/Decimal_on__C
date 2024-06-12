#ifndef S21_DECIMAL_H_
#define S21_DECIMAL_H_

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBITS 0xffffffff

typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef union {
  unsigned int uin_t;
  float floa_t;
} s21_float_bits;

typedef struct {
  uint64_t bits[7];
  uint16_t scale;
  uint8_t sign;
} big_decimal;

typedef struct {
  unsigned int bits[8];
} s21_big_decimal;

// arithmetic
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// compare
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);

// converter
int s21_from_int_to_decimal(int, s21_decimal *);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal, int *);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// other
int s21_floor(s21_decimal, s21_decimal *);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal, s21_decimal *);
int s21_negate(s21_decimal, s21_decimal *);

// general function
int s21_get_sign(s21_decimal);
void s21_set_sign(s21_decimal *, int);
int s21_get_exp(s21_decimal);
void s21_set_exp(s21_decimal *, unsigned int);
int s21_is_zero(s21_decimal);
void s21_set_zero(s21_decimal *);
s21_decimal s21_div_by_ten(s21_decimal);
int s21_mod_ten_remainder(s21_decimal);
int s21_add_decimal_mantissa(s21_decimal, s21_decimal, s21_decimal *);
s21_decimal s21_normalize_decimal(s21_decimal);
void s21_down_exp(s21_decimal a, s21_decimal *result, int diffExp);
int s21_compare_magnitude(s21_decimal value_1, s21_decimal value_2);
void s21_sub_mantissa(s21_decimal valueL, s21_decimal valueR,
                      s21_decimal *result);

// for big decimal
void s21_dec_to_big(s21_decimal dec, big_decimal *res);
big_decimal s21_sum_mantissa_big(big_decimal a, big_decimal b);
void s21_equalize_exp(s21_decimal *a, s21_decimal *b);
int s21_get_overflow(big_decimal *num);
void s21_down_exp_big(big_decimal a, big_decimal *result, int diffExp);
void s21_big_to_dec(big_decimal big, s21_decimal *res);
void s21_mult_mantissa_big(big_decimal a, big_decimal *res, int value);
s21_big_decimal s21_from_decimal_to_big_decimal(s21_decimal x);
unsigned int s21_get_big_decimal_exponent(s21_big_decimal x);
void s21_set_big_decimal_exponent(s21_big_decimal *x, unsigned int exp);
s21_big_decimal s21_normalize_big_decimal(s21_big_decimal x);
int s21_mul_big_decimal_by_10(s21_big_decimal x, s21_big_decimal *result);
s21_big_decimal s21_div_big_decimal_by_10(s21_big_decimal x);
int s21_get_big_decimal_sign(s21_big_decimal x);
void s21_set_big_decimal_sign(s21_big_decimal *x, int sign);
int s21_add_big_decimal_mantissa(s21_big_decimal x, s21_big_decimal y,
                                 s21_big_decimal *result);
void s21_sub_big_decimal_mantissa(s21_big_decimal x, s21_big_decimal y,
                                  s21_big_decimal *result);
s21_decimal s21_round_big_decimal(s21_big_decimal x, int *error);
int s21_big_decimal_is_equal(s21_big_decimal x, s21_big_decimal y);
int s21_big_decimal_is_greater(s21_big_decimal x, s21_big_decimal y);
s21_big_decimal s21_shift_big_decimal_right(s21_big_decimal x,
                                            unsigned int shift);
s21_big_decimal s21_shift_big_decimal_left(s21_big_decimal x,
                                           unsigned int shift);

#endif  // S21_DECIMAL_H_