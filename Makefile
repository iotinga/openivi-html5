BUILD_DIR ?= build
BUILD_TYPE ?= Debug 
CMAKE_ARGS ?= 

clean:
	rm -rf $(BUILD_DIR)

.PHONY: build
build:
	cmake -S . -B "$(BUILD_DIR)" -G Ninja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_ARGS) && \
	cmake --build "$(BUILD_DIR)" -j

start:
	build/openivi -u $(realpath resources/home/index.html)