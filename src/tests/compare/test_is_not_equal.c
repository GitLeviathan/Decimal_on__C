#include "../tests.h"

START_TEST(s21_is_not_equal_pos_pos_er) {
  s21_decimal var = {{50, 50, 50, 0}};
  s21_decimal var2 = {{50, 50, 50, 0}};
  int ret = s21_is_not_equal(var, var2);
  ck_assert_int_eq(ret, 0);
}
END_TEST

START_TEST(s21_is_not_equal_neg_neg_er) {
  s21_decimal var = {{50, 50, 50, 0}};
  var.bits[3] = 1U << 31;
  s21_decimal var2 = {{50, 50, 50, 0}};
  var2.bits[3] = 1U << 31;
  int ret = s21_is_not_equal(var, var2);
  ck_assert_int_eq(ret, 0);
}
END_TEST

START_TEST(s21_is_not_equal_pos_neg) {
  s21_decimal var = {{50, 50, 50, 0}};
  var.bits[3] = 1U << 31;
  s21_decimal var2 = {{50, 50, 50, 0}};
  int ret = s21_is_not_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

START_TEST(s21_is_not_equal_pos_pos) {
  s21_decimal var = {{50, 50, 50, 0}};
  s21_decimal var2 = {{50, 30, 50, 0}};
  int ret = s21_is_not_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

Suite *test_is_not_equal(void) {
  Suite *s = suite_create("\033[45m==S21_IS_NOT_EQUAL==\033[0m");
  TCase *tc = tcase_create("is_not_equal_tc");

  tcase_add_test(tc, s21_is_not_equal_pos_pos_er);
  tcase_add_test(tc, s21_is_not_equal_neg_neg_er);
  tcase_add_test(tc, s21_is_not_equal_pos_neg);
  tcase_add_test(tc, s21_is_not_equal_pos_pos);

  suite_add_tcase(s, tc);
  return s;
}