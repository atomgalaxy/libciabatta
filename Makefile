TEST_CMAKE_BUILD_DIR=cmake-build
TEST_CMAKE_INSTALL_DIR=cmake-install
TEST_CMAKE_SCRATCH_DIR=cmake-scratch


.PHONY: test
test:


.PHONY: test_cmake
test: test_cmake
test_cmake: $(TEST_CMAKE_SCRATCH_DIR)


$(TEST_CMAKE_BUILD_DIR): CMakeLists.txt test/CMakeLists.txt
	rm -rf $(TEST_CMAKE_BUILD_DIR)
	mkdir -p $(TEST_CMAKE_BUILD_DIR)
	cd $(TEST_CMAKE_BUILD_DIR) && cmake ../ -GNinja -DCMAKE_INSTALL_PREFIX="../$(TEST_CMAKE_INSTALL_DIR)" 
	cd $(TEST_CMAKE_BUILD_DIR) && ninja
	cd $(TEST_CMAKE_BUILD_DIR)/test && ctest .


$(TEST_CMAKE_INSTALL_DIR): $(TEST_CMAKE_BUILD_DIR)
	rm -rf $(TEST_CMAKE_INSTALL_DIR)
	mkdir -p $(TEST_CMAKE_INSTALL_DIR)
	cd $(TEST_CMAKE_BUILD_DIR) && ninja install


$(TEST_CMAKE_SCRATCH_DIR): $(TEST_CMAKE_INSTALL_DIR)
	rm -rf $(TEST_CMAKE_SCRATCH_DIR)
	mkdir -p $(TEST_CMAKE_SCRATCH_DIR)
	cd $(TEST_CMAKE_SCRATCH_DIR) && cp -r ../test/* .
	IDIR="$$(readlink -f $(TEST_CMAKE_INSTALL_DIR))" && \
		test -d "$$IDIR" && \
		cd $(TEST_CMAKE_SCRATCH_DIR) && \
		mkdir build && cd build/ && \
		cmake ../ -GNinja -DCMAKE_PREFIX_PATH="$$IDIR" -DCMAKE_FIND_DEBUG_MODE=ON \
		&& ninja && ninja test


.PHONY: test_bazel
test: test_bazel
test_bazel:
	bazel build //...
	bazel test //...

.PHONY: clean
clean:
	rm -rf bazel-* $(TEST_CMAKE_BUILD_DIR) $(TEST_CMAKE_SCRATCH_DIR) $(TEST_CMAKE_INSTALL_DIR)
