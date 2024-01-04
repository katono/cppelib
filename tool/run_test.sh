#!/bin/bash

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

EXIT_CODE=0

TEST_DIRS=${TEST_DIRS}" mechanism/test"
TEST_DIRS=${TEST_DIRS}" platform/test"
PROFILES=${PROFILES}" tool/profile_cppstd11"
PROFILES=${PROFILES}" tool/profile_cppstd11_noexceptions"
PROFILES=${PROFILES}" tool/profile_cppstd17"
PROFILES=${PROFILES}" tool/profile_cppstd17_noexceptions"
PROFILES=${PROFILES}" tool/profile_cppstd98"
PROFILES=${PROFILES}" tool/profile_cppstd98_noexceptions"
BUILD_TYPE="Debug Release"
for dir in ${TEST_DIRS}; do
	for pr in ${PROFILES}; do
		if [[ ${dir} =~ platform ]] && [[ ${pr} =~ cppstd98 ]]; then
			continue
		fi
		for type in ${BUILD_TYPE}; do
			conan build ${dir} --build=missing -pr ${pr} -s build_type=${type}
			if [ "$?" != "0" ]; then
				EXIT_CODE=1
			fi
		done
	done
done

exit ${EXIT_CODE}
