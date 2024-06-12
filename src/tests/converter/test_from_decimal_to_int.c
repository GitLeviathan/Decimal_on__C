#include "../tests.h"

static s21_decimal src[] = {
    /*0*/ {{0x00000001, 0x00000000, 0x00000000, 0x00000000}},  // 1 // 1
    /*1*/ {{0x0000006E, 0x00000000, 0x00000000, 0x00020000}},  // 1.10 // 1
    /*2*/ {{0x00000457, 0x00000000, 0x00000000, 0x00030000}},  // 1.111 // 1
    /*3*/ {{0x773593FF, 0x00000000, 0x00000000, 0x00090000}},  // 1.999999999 //
                                                               // 1
    /*4*/ {{0x0000007B, 0x00000000, 0x00000000, 0x00000000}},  // 123 // 123
    /*5*/ {{0x0001E078, 0x00000000, 0x00000000, 0x00030000}},  // 123.000 // 123
    /*6*/ {{0x0001E45F, 0x00000000, 0x00000000, 0x00030000}},  // 123.999 // 123
    /*7*/ {{0xFFFFFFFF, 0x000001F3, 0x00000000, 0x00030000}},  // 2147483647.999
                                                               // // 2147483647
    /*8*/ {{0x000003E7, 0x00000000, 0x00000000, 0x80030000}},  // -0.999 // 0
    /*9*/ {{0x00000001, 0x00000000, 0x00000000, 0x80000000}},  // -1 // -1
                                                               /*10*/
    {{0x000003E7, 0x000001F4, 0x00000000, 0x80030000}},  // -2147483648.999
                                                         // //
                                                         // -2147483648
    /*11*/ {{0x80000000, 0x00000000, 0x00000000, 0x80000000}},  // -2147483648
                                                                // //
                                                                // -2147483648
    /*12*/ {{0x00000000, 0x00000000, 0x00000000, 0x80000000}},  // 0 // 0
    /*13*/ {{0x00000000, 0x00000000, 0x00000000, 0x00000000}},  // 0 // 0
    /*14*/ {{0x00000097, 0x00000000, 0x00000000, 0x00010000}},  // 15.1 // 15
    /*15*/ {{0x0000060D, 0x00000000, 0x00000000, 0x00020000}},  // 15.49 // 15
    /*16*/ {{0x0000060F, 0x00000000, 0x00000000, 0x00020000}},  // 15.51 // 15
    /*17*/ {{0x00000097, 0x00000000, 0x00000000, 0x80010000}},  // -15.1 // -15
    /*18*/ {{0x0000060D, 0x00000000, 0x00000000, 0x80020000}},  // -15.49 // -15
    /*19*/ {{0x0000060F, 0x00000000, 0x00000000, 0x80020000}},  // -15.51 // -15
    /*20*/ {{0x00000619, 0x00000000, 0x00000000, 0x00010000}},  // 156.1 // 156
    /*21*/ {{0x00003D21, 0x00000000, 0x00000000, 0x00020000}},  // 156.49 // 156
    /*22*/ {{0x00003D23, 0x00000000, 0x00000000, 0x00020000}},  // 156.51 // 156
    /*23*/ {{0x00000619, 0x00000000, 0x00000000, 0x80010000}},  // -156.1 //
                                                                // -156
    /*24*/ {{0x00003D21, 0x00000000, 0x00000000, 0x80020000}},  // -156.49 //
                                                                // -156
    /*25*/ {{0x00003D23, 0x00000000, 0x00000000, 0x80020000}},  // -156.51 //
                                                                // -156
};

static int dst[] = {
    1,          1,   1,   1,

    123,        123, 123,

    2147483647, 0,   -1,  -2147483648, -2147483648, 0,   0,    15,   15,   15,
    -15,        -15, -15, 156,         156,         156, -156, -156, -156,
};

static s21_decimal src_error[] = {
    {{0xFFFFFFFF, 0x000003E7, 0x00000000,
      0x00030000}},  // 4294967295.999               //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x00000000, 0x00000001, 0x00000000,
      0x00000000}},  // 4294967296                   //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x00000000, 0x00000001, 0x00000000,
      0x80000000}},  // -4294967296                  //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x80000000, 0x00000000, 0x00000000,
      0x00000000}},  // 2147483648                   //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x80000001, 0x00000000, 0x00000000,
      0x80000000}},  // -2147483649                  //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x000F4240, 0x00002710, 0x00002710,
      0x00000000}},  // 184467440780045190120000     //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x000F4240, 0x00002710, 0x00002710,
      0x80000000}},  // -184467440780045190120000    //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x000F4240, 0x00000000, 0x00002710,
      0x00000000}},  // 184467440737095517160000     //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x000F4240, 0x00000000, 0x00002710,
      0x80000000}},  // -184467440737095517160000    //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x00002F59, 0x0CA55839, 0x00000000,
      0x00000000}},  // 911231501448785753           //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {{0x000004C2, 0x01518D64, 0x00000000,
      0x80000000}},  // -95012527787738306           //
                     // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
};

START_TEST(test) {
  for (size_t i = 0; i < sizeof(src) / sizeof(s21_decimal); ++i) {
    int tmp;
    int ret = s21_from_decimal_to_int(src[i], &tmp);
    ck_assert_int_eq(tmp, dst[i]);
    ck_assert_int_eq(ret, 0);
  }
}
END_TEST

START_TEST(error_test) {
  for (size_t i = 0; i < sizeof(src_error) / sizeof(s21_decimal); ++i) {
    int tmp;
    int ret = s21_from_decimal_to_int(src_error[i], &tmp);
    ck_assert_int_eq(ret, 1);
  }
}
END_TEST

START_TEST(s21_invalid_result) {
  s21_decimal invalid = {{0x00000000, 0x00000000, 0x00000000, 0x80000000}};
  int ret = s21_from_decimal_to_int(invalid, NULL);
  ck_assert_int_eq(ret, 1);
}
END_TEST

Suite *test_from_decimal_to_int(void) {
  Suite *s;
  TCase *tc;

  s = suite_create("\033[47m==S21_FROM_DECIMAL_TO_INT==\033[0m");
  tc = tcase_create("s21_from_decimal_to_int");

  if (s != NULL && tc != NULL) {
    tcase_add_test(tc, test);
    tcase_add_test(tc, error_test);
    tcase_add_test(tc, s21_invalid_result);
    suite_add_tcase(s, tc);
  }

  return (s);
}
