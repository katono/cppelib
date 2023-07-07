#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

EXIT_CODE=0

PACKAGE_DIRS=${PACKAGE_DIRS}" mechanism"
PACKAGE_DIRS=${PACKAGE_DIRS}" platform"
for dir in ${PACKAGE_DIRS}; do
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
done

exit ${EXIT_CODE}
