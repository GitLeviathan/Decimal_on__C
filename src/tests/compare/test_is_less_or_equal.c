#include "../tests.h"

START_TEST(s21_is_less_or_equal_eq) {
  s21_decimal var = {{500, 700, 50, 0}};
  var.bits[3] = 2 << 16;
  s21_decimal var2 = {{50, 70, 5, 0}};
  var2.bits[3] = 1 << 16;
  int ret = s21_is_less_or_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_pos_neg) {
  s21_decimal var = {{50, 20, 50, 0}};
  var.bits[3] = 2 << 16;
  s21_decimal var2 = {{50, 50, 50, 0}};
  var2.bits[3] = 7 << 16;
  var2.bits[3] |= 1U << 31;
  int ret = s21_is_less_or_equal(var, var2);
  ck_assert_int_eq(ret, 0);
}
END_TEST

START_TEST(s21_is_less_or_equal_neg_pos) {
  s21_decimal var = {{50, 20, 50, 0}};
  var.bits[3] = 1U << 31;
  var.bits[3] |= 2 << 16;
  s21_decimal var2 = {{50, 50, 50, 0}};
  var2.bits[3] = 7 << 16;
  int ret = s21_is_less_or_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_pos_pos) {
  s21_decimal var = {{50, 20, 50, 0}};
  var.bits[3] = 5 << 16;
  s21_decimal var2 = {{50, 700, 660, 0}};
  var2.bits[3] = 1 << 16;
  int ret = s21_is_less_or_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_pos_pos_er) {
  s21_decimal var = {{50, 20, 50, 0}};
  var.bits[3] = 5 << 16;
  s21_decimal var2 = {{50, 700, 660, 0}};
  var2.bits[3] = 10 << 16;
  int ret = s21_is_less_or_equal(var, var2);
  ck_assert_int_eq(ret, 0);
}
END_TEST

START_TEST(s21_is_less_or_equal_neg_neg) {
  s21_decimal var = {{50, 70, 50, 0}};
  var.bits[3] = 1U << 31;
  var.bits[3] |= 3 << 16;
  s21_decimal var2 = {{50, 50, 50, 0}};
  var2.bits[3] = 1U << 31;
  var2.bits[3] |= 5 << 16;
  int ret = s21_is_less_or_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

START_TEST(s21_is_less_or_equal_neg_neg_er) {
  s21_decimal var = {{50, 70, 5, 0}};
  var.bits[3] = 3 << 16;
  var.bits[3] |= 1U << 31;
  s21_decimal var2 = {{30, 888, 666, 0}};
  var2.bits[3] = 1U << 31;
  int ret = s21_is_less_or_equal(var, var2);
  ck_assert_int_eq(ret, 0);
}
END_TEST

Suite *test_is_less_or_equal(void) {
  Suite *s = suite_create("\033[45m==S21_IS_LESS_OR_EQUAL==\033[0m");
  TCase *tc = tcase_create("is_less_or_equal_tc");

  tcase_add_test(tc, s21_is_less_or_equal_eq);
  tcase_add_test(tc, s21_is_less_or_equal_pos_neg);
  tcase_add_test(tc, s21_is_less_or_equal_neg_pos);
  tcase_add_test(tc, s21_is_less_or_equal_pos_pos);
  tcase_add_test(tc, s21_is_less_or_equal_pos_pos_er);
  tcase_add_test(tc, s21_is_less_or_equal_neg_neg);
  tcase_add_test(tc, s21_is_less_or_equal_neg_neg_er);

  suite_add_tcase(s, tc);
  return s;
}