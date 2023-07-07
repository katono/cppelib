#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT [clean|rebuild]"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

EXIT_CODE=0

BUILD_DIR=build
PACKAGE_DIRS=${PACKAGE_DIRS}" mechanism"
PACKAGE_DIRS=${PACKAGE_DIRS}" platform"
for dir in ${PACKAGE_DIRS}; do
	if [ "$2" = "clean" ]; then
		rm -rf ${dir}/${BUILD_DIR}
		rm -rf ${dir}/test_package/${BUILD_DIR}
	else
		if [ "$2" = "rebuild" ]; then
			rm -rf ${dir}/${BUILD_DIR}
			rm -rf ${dir}/test_package/${BUILD_DIR}
		fi
		conan build ${dir} --build=missing
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
			continue
		fi
		conan create ${dir} --build=missing
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
			continue
		fi
	fi
done

exit ${EXIT_CODE}
