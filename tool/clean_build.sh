#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

BUILD_DIR=build
BUILD_DIRS=${BUILD_DIRS}" mechanism/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" mechanism/test/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" mechanism/test_package/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" platform/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" platform/test/${BUILD_DIR}"
BUILD_DIRS=${BUILD_DIRS}" platform/test_package/${BUILD_DIR}"
for dir in ${BUILD_DIRS}; do
	rm -rf ${dir}
done
