
.PHONY: test
test:


cmake-build:
	mkdir -p build/


.PHONY: test_cmake
test: test_cmake
test_cmake: cmake-build
	cd build && cmake ../ -GNinja
	cd build && ninja
	cd build && ninja test


.PHONY: test_bazel
test: test_bazel
test_bazel:
	bazel build //...
	bazel test //...
