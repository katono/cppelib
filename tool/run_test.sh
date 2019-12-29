#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT [clean|rebuild]"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

BUILD_DIR=build
BUILD_DIRS="mechanism/test/${BUILD_DIR} platform/test/cmake_StdCppOSWrapperTest/${BUILD_DIR} platform/test/cmake_PosixOSWrapperTest/${BUILD_DIR}"
for dir in ${BUILD_DIRS}; do
	if [ "$2" = "clean" ]; then
		rm -rf ${dir}
	else
		if [ "$2" = "rebuild" ]; then
			rm -rf ${dir}
		fi
		mkdir -p ${dir}
		cd ${dir}
		rm -f ../CMakeCache.txt
		cmake ..
		make
		ctest -V
		cd -
	fi
done

