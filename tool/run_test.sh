#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT [clean|rebuild]"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

EXIT_CODE=0

BUILD_DIR=build
BUILD_DIRS="mechanism/test/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" mechanism/test/AssertionNoExceptionsTest/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" mechanism/test/ContainerNoExceptionsTest/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" platform/test/cmake_StdCppOSWrapperTest/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" platform/test/cmake_PosixOSWrapperTest/${BUILD_DIR}"
for dir in ${BUILD_DIRS}; do
	if [ "$2" = "clean" ]; then
		rm -rf ${dir}
	else
		if [ "$2" = "rebuild" ]; then
			rm -rf ${dir}
		fi
		mkdir -p ${dir}
		cd ${dir}
		cmake ..
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
			cd -
			continue
		fi
		make
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
			cd -
			continue
		fi
		ctest -V
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
			cd -
			continue
		fi
		cd -
	fi
done

exit ${EXIT_CODE}
