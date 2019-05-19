#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

BUILD_DIR=coverage_build
BUILD_DIRS="mechanism/test/${BUILD_DIR} platform/test/cmake_StdCppOSWrapperTest/${BUILD_DIR} platform/test/cmake_PosixOSWrapperTest/${BUILD_DIR}"
for dir in ${BUILD_DIRS}; do
	rm -rf ${dir}
	mkdir -p ${dir}
	cd ${dir}
	cmake .. -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_EXE_LINKER_FLAGS=--coverage -DCMAKE_BUILD_TYPE=Debug
	make
	ctest -V
	cd -
done

lcov -d . -c -o coverage.info
lcov -r coverage.info "*/CppUTest/*" "/usr/*" "*/test/*" -o coverage.info
genhtml -o coverage_report --num-spaces 4 -s --legend coverage.info

