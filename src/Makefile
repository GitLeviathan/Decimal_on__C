CC = gcc -Wall -Werror -Wextra -std=c11 # -pedantic
TESTS = ./tests/*.c ./tests/*.h ./tests/compare/*.c ./tests/other/*.c ./tests/converter/*.c ./tests/arithmetic/*.c

ifeq ($(shell uname -s),Linux)
LFLAGS=-lcheck -lsubunit -lm -lrt -lpthread
endif
ifeq ($(shell uname -s),Darwin)
LFLAGS=-lcheck -lm -lpthread
endif

SOURCES = $(wildcard s21_*.c)
OBJECTS = $(SOURCES:.c=.o)

all: clean test gcov_report

s21_decimal.a: $(OBJECTS)
	ar rcs $@ $^
	rm -f */s21_*.o

%.o: %.c
	$(CC) -c $^ -o $@

test: $(TESTS) ./tests/*.h s21_decimal.h s21_decimal.a 
	$(CC) -o $@ $^ $(LFLAGS)

rebuild: clean all

clean:
	rm -rf *.o *.gcno *.info *.gcda report s21_decimal.a test

lint:
	clang-format --style=Google *.c *.h **/*.c **/*.h **/**/*.c -n

lint-fix:
	clang-format --style=Google -i *.c *.h **/*.c **/*.h **/**/*.c

gcov_report: s21_decimal.a
	$(CC) --coverage $(SOURCES) $(TESTS) -o test $(LFLAGS)
	./test
	lcov -t "test" -o report.info -c -d .
	genhtml -o report report.info
	xdg-open ./report/index.html
	rm -f *.gcno *.info *.gcda

# $@: $^ - объект сборки: исходники
# ar - это утилита архивации, используемая для создания и манипуляции архивами файлов
# rcs - это опции команды ar:
# r - добавление файлов к архиву или обновление файлов в архиве
# c - создание нового архива, если он не существует
# s - создание индекса архива для быстрого доступа
# -fprofile-arcs -ftest-coverage - анализ покрытия