CC = gcc
CFLAGS = -Wall -Wextra -O2 -I.
CFLAGS_COV = -Wall -Wextra -O0 -g -I. -fprofile-arcs -ftest-coverage
AR = ar
ARFLAGS = rcs

LIBRARY = libring.a
SOURCES = ring.c
OBJECTS = $(SOURCES:.c=.o)
OBJECTS_COV = $(SOURCES:.c=.gc.o)
HEADER = ring.h

TEST_SOURCE = test.c
TEST_EXEC = test_ring
TEST_EXEC_COV = test_ring_cov
TEST_OBJECTS = test_ring.o
TEST_OBJECTS_COV = test_ring.gc.o

COVERAGE_DIR = coverage
COVERAGE_INFO = $(COVERAGE_DIR)/coverage.info
COVERAGE_HTML = $(COVERAGE_DIR)/html

# Цели по умолчанию
all: $(LIBRARY) example

# Сборка библиотеки
$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

# Пример программы
example: example.c $(LIBRARY)
	$(CC) $(CFLAGS) example.c -L. -lring -o example

# Тестирование без покрытия
test: $(LIBRARY) $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): $(TEST_SOURCE) $(LIBRARY)
	$(CC) $(CFLAGS) $(TEST_SOURCE) -L. -lring -o $@

# Тестирование с покрытием кода
test-report: $(TEST_EXEC_COV)
	./$(TEST_EXEC_COV)
	@mkdir -p $(COVERAGE_DIR)
	@gcov -b -c -p ring.c test_ring.c > $(COVERAGE_DIR)/gcov_report.txt 2>&1
	@echo "\n📈 Отчет gcov создан в $(COVERAGE_DIR)/gcov_report.txt"
	@echo "📊 Сводка покрытия:"
	@gcov -b ring.c | grep -A 5 "Lines executed:"
	@if command -v lcov >/dev/null 2>&1; then \
		echo "\n🌐 Генерация HTML отчета..."; \
		lcov --capture --directory . --output-file $(COVERAGE_INFO) --rc lcov_branch_coverage=1; \
		lcov --remove $(COVERAGE_INFO) '/usr/*' 'test_*.c' --output-file $(COVERAGE_INFO); \
		genhtml $(COVERAGE_INFO) --output-directory $(COVERAGE_HTML) --branch-coverage; \
		echo "✅ HTML отчет создан в $(COVERAGE_HTML)/index.html"; \
	else \
		echo "\n⚠️  lcov не установлен. Установите для генерации HTML отчета:"; \
		echo "   Ubuntu: sudo apt-get install lcov"; \
		echo "   MacOS: brew install lcov"; \
	fi

$(TEST_EXEC_COV): $(SOURCES) $(TEST_SOURCE) $(HEADER)
	$(CC) $(CFLAGS_COV) -c ring.c -o ring.gc.o
	$(CC) $(CFLAGS_COV) -c test_ring.c -o test_ring.gc.o
	$(CC) $(CFLAGS_COV) ring.gc.o test_ring.gc.o -o $@

# Быстрая проверка покрытия (без lcov)
coverage-quick: $(TEST_EXEC_COV)
	./$(TEST_EXEC_COV)
	@gcov -b ring.c | grep -E "(File|Lines|Branches)"

# Очистка
clean:
	rm -f $(OBJECTS) $(OBJECTS_COV) $(TEST_OBJECTS) $(TEST_OBJECTS_COV)
	rm -f $(LIBRARY) $(TEST_EXEC) $(TEST_EXEC_COV) example
	rm -f *.gcov *.gcda *.gcno
	rm -rf $(COVERAGE_DIR) *.dSYM

# Установка/удаление
install: $(LIBRARY)
	cp $(LIBRARY) /usr/local/lib/
	cp $(HEADER) /usr/local/include/

uninstall:
	rm -f /usr/local/lib/$(LIBRARY) /usr/local/include/$(HEADER)

# Дополнительные цели
check: test
	@echo "✅ Тесты пройдены"

valgrind: $(TEST_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TEST_EXEC)

.PHONY: all test test-report coverage-quick clean install uninstall check valgrind
