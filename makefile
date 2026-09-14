CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2
COVFLAGS = -std=c++17 -Wall -Wextra -Werror -O0 -g --coverage
LDLIBS   = -lglut -lGLU -lGL
INC      = -Isource/include -Itests
SRC      = $(wildcard source/include/*.cpp)
TESTS    = $(wildcard tests/*.cpp)
COV_DIR  = build/coverage
COV_INFO = $(COV_DIR)/coverage.info
# Everything clang-format owns; the vendored tests/doctest.h is excluded.
FMT_SRC  = source/main.cpp $(wildcard source/include/*.h) $(SRC) \
           $(wildcard tests/test_*.cpp) tests/fake_renderer.h

.PHONY: build run lint format test coverage clean

build: build/submarine

build/submarine: source/main.cpp $(SRC) $(wildcard source/include/*.h)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INC) source/main.cpp $(SRC) $(LDLIBS) -o $@

run: build/submarine
	./build/submarine

lint:
	clang-format --dry-run -Werror $(FMT_SRC)
	clang-tidy --quiet source/main.cpp $(SRC) -- $(CXXFLAGS) $(INC)

format:
	clang-format -i $(FMT_SRC)

build/run_tests: $(SRC) $(TESTS) $(wildcard source/include/*.h) tests/fake_renderer.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INC) $(SRC) $(TESTS) $(LDLIBS) -o $@

test: build/run_tests
	xvfb-run -a ./build/run_tests

$(COV_DIR)/run_tests: $(SRC) $(TESTS) $(wildcard source/include/*.h) tests/fake_renderer.h
	@mkdir -p $(COV_DIR)
	$(CXX) $(COVFLAGS) $(INC) $(SRC) $(TESTS) $(LDLIBS) -o $@

coverage: $(COV_DIR)/run_tests
	find $(COV_DIR) -name '*.gcda' -delete
	xvfb-run -a ./$(COV_DIR)/run_tests
	lcov --quiet --capture --directory $(COV_DIR) --output-file $(COV_INFO) --exclude '/usr/*' --exclude '*/tests/*'
	lcov --list $(COV_INFO)
# lcov 2.0's --list prints nonsense per-file percentages; --summary below is the
# real gate, and genhtml's report is the one to read.
	@lcov --summary $(COV_INFO) 2>&1 | grep -E '^\s+lines' | grep -q '100.0%' || (echo "FAIL: line coverage below 100%"; exit 1)
	genhtml --quiet $(COV_INFO) --output-directory $(COV_DIR)/html
	@echo "HTML report: $(COV_DIR)/html/index.html"

clean:
	rm -rf build
