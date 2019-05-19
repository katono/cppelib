#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT [clean]"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

rm -rf coverage_report
rm -f coverage.info

BUILD_DIR=coverage_build
BUILD_DIRS="mechanism/test/${BUILD_DIR} platform/test/cmake_StdCppOSWrapperTest/${BUILD_DIR} platform/test/cmake_PosixOSWrapperTest/${BUILD_DIR}"
for dir in ${BUILD_DIRS}; do
	if [ "$2" = "clean" ]; then
		rm -rf ${dir}
	else
		rm -rf ${dir}
		mkdir -p ${dir}
		cd ${dir}
		cmake .. -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_EXE_LINKER_FLAGS=--coverage -DCMAKE_BUILD_TYPE=Debug
		make
		ctest -V
		cd -
	fi
done

if [ "$2" = "clean" ]; then
	exit
fi

lcov -d . -c -o coverage.info
lcov -r coverage.info "*/CppUTest/*" "/usr/*" "*/test/*" -o coverage.info
genhtml -o coverage_report --num-spaces 4 -s --legend coverage.info

