BUILD_DIR = ./build

.PHONY: all
all: build

.PHONY: buildsystem
buildsystem:
	cmake -B $(BUILD_DIR) -G Ninja

.PHONY: build
build: buildsystem
	cmake --build build

.PHONY: re
re: fclean build

.PHONY: qtest
qtest: build
	GTEST_COLOR=1 ctest --test-dir build $(CTEST_OPT)

.PHONY: test
test: CTEST_OPT += -V
test: qtest

.PHONY: clean
clean:
	rm -rf $(FULL_PATH)

.PHONY: fclean
fclean: clean
	rm -rf $(BUILD_DIR)

.PHONY: format
format:
	clang-format -i $(shell find FileReader -name '*.c' -or -name '*.h')
	clang-format -i $(shell find test -name '*.cpp' -or -name '*.hpp' -or -name '*.h')

# aliases
.PHONY: b c fc t qt fmt
b: build
c: clean
fc: fclean
t: test
qt: qtest
fmt: format

-include $(DEPS)
