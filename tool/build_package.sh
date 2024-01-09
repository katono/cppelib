#!/bin/bash

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

EXIT_CODE=0

PACKAGE_DIRS=${PACKAGE_DIRS}" mechanism"
PACKAGE_DIRS=${PACKAGE_DIRS}" platform"
PROFILES=${PROFILES}" tool/profiles/profile_cppstd11"
PROFILES=${PROFILES}" tool/profiles/profile_cppstd11_noexceptions"
for dir in ${PACKAGE_DIRS}; do
	for pr in ${PROFILES}; do
		conan build ${dir} --build=missing -pr ${pr}
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
			continue
		fi
		conan create ${dir} --build=missing -pr ${pr}
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
			continue
		fi
	done
done

exit ${EXIT_CODE}
