#include "../tests.h"

START_TEST(s21_is_equal_pos_pos) {
  s21_decimal var = {{50, 50, 50, 0}};
  s21_decimal var2 = {{50, 50, 50, 0}};
  int ret = s21_is_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

START_TEST(s21_is_equal_neg_neg) {
  s21_decimal var = {{50, 50, 50, 0}};
  var.bits[3] = 1U << 31;
  s21_decimal var2 = {{50, 50, 50, 0}};
  var2.bits[3] = 1U << 31;
  int ret = s21_is_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

START_TEST(s21_is_equal_er_pos_neg) {
  s21_decimal var = {{50, 50, 50, 0}};
  var.bits[3] = 1U << 31;
  s21_decimal var2 = {{50, 50, 50, 0}};
  int ret = s21_is_equal(var, var2);
  ck_assert_int_eq(ret, 0);
}
END_TEST

START_TEST(s21_is_equal_er) {
  s21_decimal var = {{50, 50, 50, 0}};
  s21_decimal var2 = {{50, 30, 50, 0}};
  int ret = s21_is_equal(var, var2);
  ck_assert_int_eq(ret, 0);
}
END_TEST

START_TEST(s21_is_equal_1) {
  s21_decimal var = {{0, 0, 0, 0}};  // -0.0000000000
  var.bits[3] = 10 << 16;
  var.bits[3] = 1U << 31;
  s21_decimal var2 = {{0, 0, 0, 0}};  // 0.000
  var2.bits[3] = 3 << 16;
  int ret = s21_is_equal(var, var2);
  ck_assert_int_eq(ret, 1);
}
END_TEST

Suite *test_is_equal(void) {
  Suite *s = suite_create("\033[45m==S21_IS_EQUAL==\033[0m");
  TCase *tc = tcase_create("is_equal_tc");

  tcase_add_test(tc, s21_is_equal_er);
  tcase_add_test(tc, s21_is_equal_er_pos_neg);
  tcase_add_test(tc, s21_is_equal_neg_neg);
  tcase_add_test(tc, s21_is_equal_pos_pos);
  tcase_add_test(tc, s21_is_equal_1);

  suite_add_tcase(s, tc);
  return s;
}