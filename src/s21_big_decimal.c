#include "s21_decimal.h"

#define NULL_big_decimal       \
  (s21_big_decimal) {          \
    { 0, 0, 0, 0, 0, 0, 0, 0 } \
  }

void s21_mult_mantissa_big(big_decimal a, big_decimal* res, int value) {
  // умножает мантису на заданный инт
  uint64_t carry = 0;
  for (int i = 0; i < 7; i++) {
    uint64_t mul = a.bits[i] * value + carry;
    res->bits[i] = (uint32_t)mul;
    carry = mul >> 32;
  }
}

void s21_big_to_dec(big_decimal big, s21_decimal* res) {
  //  из БигДецимала в обычный децимал
  for (int i = 0; i < 3; i++) {
    res->bits[i] = (unsigned int)big.bits[i];
  }
  s21_set_exp(res, big.scale);
  s21_set_sign(res, big.sign);
}

big_decimal s21_div_by_ten_big(big_decimal value, int* a) {
  // делит на 10 бигдец;
  big_decimal result = value;
  uint64_t remainder = 0;
  for (int i = 6; i >= 0; i--) {
    // сдвигаем remainder на 32 бита влево, чтобы освободить место для
    // value.bits[i], и затем они объединяются оператором "или" ("|").
    uint64_t numerator = (remainder << 32) | value.bits[i];
    result.bits[i] = (uint32_t)(numerator / 10);
    remainder = numerator % 10;
  }
  *a = (int)remainder;

  return result;
}

int s21_truncate_big(big_decimal value, big_decimal* result) {
  // отбрасывает всю дробную часть
  int res = 0;
  int a = 0;
  int exp = value.scale;
  big_decimal temp = value;
  // Делим на 10 в степени exp, отнимая от bits[3] в каждом шаге
  for (; exp > 0; exp--) {
    temp = s21_div_by_ten_big(temp, &a);
    value.scale -= 1;
  }
  if (temp.bits[0] == 0) temp.bits[3] = 0;
  *result = temp;
  return res;
}

void s21_down_exp_big(big_decimal a, big_decimal* result, int diffExp) {
  // понижение степени с банковским округлением,
  // diffExp - на сколько понизить степень
  big_decimal c = {0};
  s21_truncate_big(a, &c);
  int A = 0;
  while (diffExp > 0) {
    *result = s21_div_by_ten_big(a, &A);
    diffExp--;
    result->scale -= 1;
    if (A >= 5) {
      result->bits[0] += 1;
    }
  }
}

void s21_dec_to_big(s21_decimal dec,
                    big_decimal* res) {  // из децимала в БигДецимал
  for (int i = 0; i < 3; i++) {
    res->bits[i] = dec.bits[i] & MAXBITS;
  }
  res->scale = s21_get_exp(dec);
  res->sign = s21_get_sign(dec);
}

s21_big_decimal s21_from_decimal_to_big_decimal(s21_decimal x) {
  s21_big_decimal res = NULL_big_decimal;
  for (int i = 0; i < 3; i++) {
    res.bits[i] = x.bits[i];
  }
  res.bits[7] = x.bits[3];
  return res;
}

unsigned int s21_get_big_decimal_exponent(s21_big_decimal x) {
  return (x.bits[7] << 1) >> 17;
}

void s21_set_big_decimal_exponent(s21_big_decimal* x, unsigned int exp) {
  x->bits[7] &= ~(((1 << 8) - 1) << 16);
  x->bits[7] |= exp << 16;
}

s21_big_decimal s21_div_big_decimal_by_10(s21_big_decimal x) {
  uint64_t prev_rem = 0;
  for (int i = 6; i >= 0; i--) {
    uint64_t rem = (x.bits[i] + prev_rem) % 10 << 32;
    x.bits[i] = (x.bits[i] + prev_rem) / 10;
    prev_rem = rem;
  }
  s21_set_big_decimal_exponent(&x, s21_get_big_decimal_exponent(x) - 1);
  return x;
}

int s21_get_big_decimal_rem_10(s21_big_decimal x) {
  int res = 0;
  for (int i = 6; i >= 0; i--) {
    res += x.bits[i] % 10 * pow(6, i);
  }
  return res % 10;
}

s21_big_decimal s21_normalize_big_decimal(s21_big_decimal x) {
  int check = 1;
  for (int i = 0; i < 7; i++) {
    check *= x.bits[i] == 0;
  }
  if (check) {
    x = NULL_big_decimal;
  } else {
    while (s21_get_big_decimal_rem_10(x) == 0 &&
           s21_get_big_decimal_exponent(x) != 0) {
      x = s21_div_big_decimal_by_10(x);
    }
  }
  return x;
}

int s21_mul_big_decimal_by_10(s21_big_decimal x, s21_big_decimal* result) {
  int error = 0;

  s21_big_decimal shift3 = s21_shift_big_decimal_left(x, 3);
  error = !s21_big_decimal_is_equal(x, s21_shift_big_decimal_right(shift3, 3));

  s21_big_decimal shift1 = s21_shift_big_decimal_left(x, 1);
  error |= s21_add_big_decimal_mantissa(shift3, shift1, result);

  s21_set_big_decimal_exponent(result, s21_get_big_decimal_exponent(x) + 1);

  return error;
}

int s21_get_big_decimal_sign(s21_big_decimal x) { return x.bits[7] >> 31; }

void s21_set_big_decimal_sign(s21_big_decimal* x, int sign) {
  x->bits[7] = (sign) ? (x->bits[7] | (1U << 31)) : (x->bits[7] & ~(1U << 31));
}

int s21_add_big_decimal_mantissa(s21_big_decimal x, s21_big_decimal y,
                                 s21_big_decimal* result) {
  int excess = 0;
  for (int i = 0; i < 7; i++) {
    uint64_t buff = (uint64_t)x.bits[i] + y.bits[i] + excess;
    if (buff != (uint32_t)buff) {
      result->bits[i] = buff - ((uint64_t)1 << 32);
      excess = 1;
    } else {
      result->bits[i] = buff;
      excess = 0;
    }
  }
  return excess;
}

void s21_sub_big_decimal_mantissa(s21_big_decimal x, s21_big_decimal y,
                                  s21_big_decimal* result) {
  int flaw = 0;
  for (int i = 0; i < 7; i++) {
    uint64_t buff = (uint64_t)x.bits[i] - y.bits[i] - flaw;
    if (buff != (uint32_t)buff) {
      result->bits[i] = buff + ((uint64_t)1 << 32);
      flaw = 1;
    } else {
      result->bits[i] = buff;
      flaw = 0;
    }
  }
}

s21_decimal s21_from_big_decimal_to_decimal(s21_big_decimal x) {
  s21_decimal res;
  s21_set_zero(&res);
  for (int i = 0; i < 3; i++) {
    res.bits[i] = x.bits[i];
  }
  res.bits[3] = x.bits[7];
  return res;
}

int s21_big_decimal_check_conversion(s21_big_decimal x) {
  int res = 0;
  for (int i = 3; i < 7; i++) {
    res += x.bits[i] != 0;
  }
  return res == 0;
}

s21_decimal s21_round_big_decimal(s21_big_decimal x, int* error) {
  s21_decimal result;
  s21_set_zero(&result);
  x = s21_normalize_big_decimal(x);
  s21_big_decimal rem = x;
  int k = 0;

  while (s21_get_big_decimal_exponent(rem) > 28 ||
         (!s21_big_decimal_check_conversion(rem) &&
          s21_get_big_decimal_exponent(rem) != 0)) {
    rem = s21_div_big_decimal_by_10(rem);
    k++;
  }

  result = s21_from_big_decimal_to_decimal(rem);

  if (!s21_big_decimal_check_conversion(rem)) {
    *error = 1 + s21_get_big_decimal_sign(x);
  }

  for (int i = 0; i < k; i++) {
    s21_mul_big_decimal_by_10(rem, &rem);
  }
  s21_sub_big_decimal_mantissa(x, rem, &rem);

  s21_big_decimal five = {{5, 0, 0, 0, 0, 0, 0, 0}};
  s21_set_big_decimal_sign(&rem, 0);

  for (int i = 0; i < k - 1; i++) {
    s21_mul_big_decimal_by_10(five, &five);
  }

  s21_set_big_decimal_exponent(&five, s21_get_big_decimal_exponent(rem));

  if (s21_big_decimal_is_greater(rem, five) ||
      (s21_big_decimal_is_equal(rem, five) &&
       s21_mod_ten_remainder(result) % 2)) {
    if (s21_add_decimal_mantissa(result, ((s21_decimal){{1, 0, 0, 0}}),
                                 &result)) {
      *error = 1 + s21_get_big_decimal_sign(x);
    }
  }

  result = s21_normalize_decimal(result);

  return result;
}

int s21_big_decimal_is_equal(s21_big_decimal x, s21_big_decimal y) {
  x = s21_normalize_big_decimal(x);
  y = s21_normalize_big_decimal(y);
  int res = 1;
  for (int i = 0; i < 8; i++) {
    if (x.bits[i] != y.bits[i]) res = 0;
  }
  return res;
}

int s21_big_decimal_is_greater(s21_big_decimal x, s21_big_decimal y) {
  int res = 0;
  int stop = 0;
  int sign_x = s21_get_big_decimal_sign(x);
  int sign_y = s21_get_big_decimal_sign(y);

  if (sign_x != sign_y) {
    res = sign_y;
    stop = 1;
  }

  s21_big_decimal* p_min_exp =
      (s21_get_big_decimal_exponent(x) > s21_get_big_decimal_exponent(y)) ? &y
                                                                          : &x;
  while (abs((int)s21_get_big_decimal_exponent(x) -
             (int)s21_get_big_decimal_exponent(y))) {
    s21_mul_big_decimal_by_10(*p_min_exp, p_min_exp);
  }

  for (int i = 6; !stop && i >= 0; i--) {
    if (x.bits[i] != y.bits[i]) {
      stop = 1;
      res = (x.bits[i] > y.bits[i]) != sign_x;
    }
  }

  return res;
}

s21_big_decimal s21_shift_big_decimal_right(s21_big_decimal x,
                                            unsigned int shift) {
  for (unsigned int i = 0; i < shift; i++) {
    for (int bit = 0; bit < 6; bit++) {
      x.bits[bit] >>= 1;
      x.bits[bit] |= (x.bits[bit + 1] & 1) << 31;
    }
    x.bits[6] >>= 1;
  }
  return x;
}

s21_big_decimal s21_shift_big_decimal_left(s21_big_decimal x,
                                           unsigned int shift) {
  for (unsigned int i = 0; i < shift; i++) {
    for (int bit = 6; bit > 0; bit--) {
      x.bits[bit] <<= 1;
      x.bits[bit] |= (x.bits[bit - 1] & (1U << 31)) >> 31;
    }
    x.bits[0] <<= 1;
  }
  return x;
}

big_decimal s21_sum_mantissa_big(big_decimal a,
                                 big_decimal b) {  // складывает мантисы
  big_decimal result = {0};
  uint64_t carry = 0;
  for (int i = 0; i < 7; i++) {
    uint64_t mul = a.bits[i] + b.bits[i] + carry;
    result.bits[i] = (uint32_t)mul;
    carry = mul >> 32;
  }
  return result;
}
