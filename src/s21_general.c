#include "s21_decimal.h"

// Функция для получения знака числа (1: минус / 0: плюс)
int s21_get_sign(s21_decimal num) { return (num.bits[3] >> 31); }

void s21_set_sign(s21_decimal* num, int sign) {
  num->bits[3] =
      (sign) ? (num->bits[3] | (1U << 31)) : (num->bits[3] & ~(1U << 31));
}

int s21_get_exp(s21_decimal value) { return (value.bits[3] >> 16) & 0xFF; }

void s21_set_exp(s21_decimal* x, unsigned int exp) {
  x->bits[3] = (x->bits[3] & ~(0xFF << 16)) | (exp << 16);
}

// Функция для проверки, является ли число нулем
int s21_is_zero(s21_decimal value_1) {
  int check = 1;
  for (int i = 0; i < 3; i++) {
    check *= value_1.bits[i] == 0;
  }
  return check;
}

void s21_set_zero(s21_decimal* dst) {
  if (dst) {
    for (int i = 0; i < 4; i++) {
      dst->bits[i] = 0;
    }
  }
}

s21_decimal s21_div_by_ten(s21_decimal value) {
  s21_decimal result = value;
  uint64_t remainder = 0;
  for (int i = 2; i >= 0; i--) {
    // сдвигаем remainder на 32 бита влево, чтобы освободить место для
    // value.bits[i], и затем они объединяются оператором "или" ("|").
    uint64_t numerator = (remainder << 32) | value.bits[i];
    result.bits[i] = (uint32_t)(numerator / 10);
    remainder = numerator % 10;
  }
  return result;
}

int s21_mod_ten_remainder(s21_decimal x) {
  uint64_t remainder = 0;
  for (int i = 2; i >= 0; i--) {
    uint64_t mantissa = (remainder << 32) | x.bits[i];
    remainder = mantissa % 10;
  }
  return remainder;
}

int s21_add_decimal_mantissa(s21_decimal valueL, s21_decimal valueR,
                             s21_decimal* result) {
  int excess = 0;

  for (int i = 0; i < 3; i++) {
    uint64_t sum = (uint64_t)valueL.bits[i] + valueR.bits[i] + excess;
    result->bits[i] = sum & 0xFFFFFFFF;  // Наименее значащие 32 бита суммы
    excess = sum >> 32;  // Перенос для следующего разряда
  }

  return excess;
}

s21_decimal s21_normalize_decimal(s21_decimal value) {
  if (s21_is_zero(value)) {
    s21_set_zero(&value);
  } else {
    while (s21_mod_ten_remainder(value) == 0 && s21_get_exp(value) != 0) {
      value = s21_div_by_ten(value);
      s21_set_exp(&value, s21_get_exp(value) - 1);
    }
  }
  return value;
}

int s21_first_bit(s21_decimal a) {  // получение Первого бита в децимале
  return (a.bits[0] << 31) >> 31;
}

void s21_down_exp(s21_decimal a, s21_decimal* result,
                  int diffExp) {  // понижение степени с банковским округлением,
                                  // diffExp - на сколько понизить степень
  s21_decimal c = {0};
  s21_truncate(a, &c);
  int nopar = s21_first_bit(c);
  while (diffExp > 0) {
    int littleRound = a.bits[0] % 10;
    *result = s21_div_by_ten(*result);
    diffExp--;
    result->bits[3] -= 65536;
    if (littleRound >= 5 && nopar) {
      result->bits[0] += 1;
    }
  }
}

void s21_sub_mantissa(s21_decimal valueL, s21_decimal valueR,
                      s21_decimal* result) {
  // разность мантисс
  s21_decimal temp = valueL;  // создаем структцрц temt и приравниваем ее к
                              // структуре уменьшаемого
  uint64_t term1 =
      temp.bits[0];  // создаем переменную с 64 битами и приравниваем ее к
                     // нулевому биту уменьшаемого
  for (int i = 0; i < 3; i++) {  // идем от младшего битса к старшему
    if (temp.bits[i] <
        valueR.bits[i]) {  // если уменьшаемый битс меньше вычитаемого
      temp.bits[i + 1] -= 1;  // то старший бит уменьшаем на 1
      term1 = (uint64_t)temp.bits[i] +
              4294967296;  // текущий бит кладем во временную переменную и плюс
    }
    uint64_t sub = term1 - valueR.bits[i];
    term1 = temp.bits[i + 1];
    result->bits[i] = (uint32_t)sub;
  }
}

void s21_set_exp_big(big_decimal* x, unsigned int exp) {
  // установить степень в БигДецимале
  x->scale = exp;
}

void s21_equalize_exp(s21_decimal* a, s21_decimal* b) {
  // выравнивает степени чисел с проверкой переполнения при
  // повыщени степени числа
  int scale_a = s21_get_exp(*a);
  int scale_b = s21_get_exp(*b);
  int diff = scale_a - scale_b;
  int flag_diff = 0;
  big_decimal res = {0};
  big_decimal check = {0};
  if (!s21_is_zero(*a) && !s21_is_zero(*b)) {
    if (diff > 0) {  // степень a > степени b
      s21_dec_to_big(*b, &res);
      s21_mult_mantissa_big(res, &check, 10);
      while (diff > 0 && scale_b < 28) {
        if (s21_get_overflow(&check) ||
            s21_get_overflow(&res)) {  // проверяем что при повышении степени
                                       // числа не происходит переполнения
          flag_diff = diff;  // если переполнение, то флагу указываем значение
                             // на сколько разница осталась
          diff = 0;  // и выходим из while
        } else {
          s21_mult_mantissa_big(res, &res, 10);
          scale_b++;
          diff--;
        }
      }
      s21_set_exp_big(&res, scale_b);
      s21_big_to_dec(res, b);
      if (flag_diff > 0) {  // проверяем нужно ли понижать степень второго числа
                            // из-за переполнения первого при повышении степени
        s21_down_exp(*a, a, flag_diff);
      }
    } else if (diff < 0 && scale_a < 28) {  // a < b
      s21_dec_to_big(*a, &res);
      s21_mult_mantissa_big(res, &check, 10);
      while (diff < 0 && scale_a < 28) {
        if (s21_get_overflow(&check) ||
            s21_get_overflow(&res)) {  // проверяем что при повышении степени
                                       // числа не происходит переполнения
          flag_diff = abs(diff);  // если переполнение, то флагу указываем
                                  // значение на сколько разница осталась
          diff = 0;  // и выходим из while
        } else {
          s21_mult_mantissa_big(res, &res, 10);
          scale_a++;
          diff++;
        }
      }
      s21_set_exp_big(&res, scale_a);
      s21_big_to_dec(res, a);
      if (flag_diff > 0) {  // проверяем нужно ли опнижать степень второго числа
                            // из-за переполнения первого при повышении степени
        s21_down_exp(*b, b, flag_diff);
      }
    }
  }
}