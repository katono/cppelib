#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

cd mechanism/test
cmake . -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_EXE_LINKER_FLAGS=--coverage -DCMAKE_BUILD_TYPE=Debug
make
ctest -V
cd -

cd platform/test/cmake_StdCppOSWrapperTest
cmake . -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_EXE_LINKER_FLAGS=--coverage -DCMAKE_BUILD_TYPE=Debug
make
ctest -V
cd -

cd platform/test/cmake_PosixOSWrapperTest
cmake . -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_EXE_LINKER_FLAGS=--coverage -DCMAKE_BUILD_TYPE=Debug
make
ctest -V
cd -


lcov -d . -c -o coverage.info
lcov -r coverage.info "*/CppUTest/*" "/usr/*" "*/test/*" -o coverage.info
genhtml -o coverage_report --num-spaces 4 -s --legend coverage.info

