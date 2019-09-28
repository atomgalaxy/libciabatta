ROOT:=$(shell pwd)
TEST_DIR=$(ROOT)/.test-output

.PHONY: test
test:


.PHONY: test_cmake
test: test_cmake
test_cmake: $(TEST_DIR)/cmake-scratch


.PHONY: test_cmake_add_subdir
test: test_cmake_add_subdir
test_cmake_add_subdir: $(TEST_DIR)/ciabatta-copy


$(TEST_DIR)/cmake-build: CMakeLists.txt test/CMakeLists.txt
	rm -rf $(TEST_DIR)/cmake-build
	mkdir -p $(TEST_DIR)/cmake-build
	cd $(TEST_DIR)/cmake-build && \
		cmake $(ROOT) -GNinja \
		-DCMAKE_INSTALL_PREFIX="$(TEST_DIR)/cmake-install"
	cd $(TEST_DIR)/cmake-build && ninja
	cd $(TEST_DIR)/cmake-build/test && ctest .


$(TEST_DIR)/cmake-install: $(TEST_DIR)/cmake-build
	rm -rf $(TEST_DIR)/cmake-install
	mkdir -p $(TEST_DIR)/cmake-install
	cd $(TEST_DIR)/cmake-build && ninja install


$(TEST_DIR)/cmake-scratch: $(TEST_DIR)/cmake-install
	rm -rf $(TEST_DIR)/cmake-scratch
	mkdir -p $(TEST_DIR)/cmake-scratch/
	cp -r test/* $(TEST_DIR)/cmake-scratch/
	mkdir -p $(TEST_DIR)/cmake-scratch/build
	cd $(TEST_DIR)/cmake-scratch/build && \
		cmake $(TEST_DIR)/cmake-scratch/ -GNinja \
			-DCMAKE_PREFIX_PATH="$(TEST_DIR)/cmake-install" \
			-DCMAKE_FIND_DEBUG_MODE=ON \
			-DCIABATTA_TEST_FIND_PACKAGE=ON
	cd $(TEST_DIR)/cmake-scratch/build && ninja && ninja test


$(TEST_DIR)/ciabatta-copy: CMakeLists.txt test/CMakeLists.txt
	rm -rf $(TEST_DIR)/ciabatta-copy
	mkdir -p $(TEST_DIR)/ciabatta-copy/thirdparty/ciabatta
	mkdir -p $(TEST_DIR)/ciabatta-copy/build/
	cp -r test/* $(TEST_DIR)/ciabatta-copy/
	cp -r ./* $(TEST_DIR)/ciabatta-copy/thirdparty/ciabatta/
	cd $(TEST_DIR)/ciabatta-copy/build && \
		cmake $(TEST_DIR)/ciabatta-copy -GNinja \
			-DCIABATTA_TEST_ADD_SUBDIRECTORY=ON \
			-DCIABATTA_TEST_ADD_SUBDIRECTORY_PATH=thirdparty/ciabatta/
	cd $(TEST_DIR)/ciabatta-copy/build && ninja
	cd $(TEST_DIR)/ciabatta-copy/build && ctest .


.PHONY: test_bazel
test: test_bazel
test_bazel:
	bazel build //...
	bazel test //...


.PHONY: clean
clean:
	rm -rf \
		bazel-* \
		$(TEST_DIR)

