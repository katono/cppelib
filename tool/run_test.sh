#!/bin/bash

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

EXIT_CODE=0

PROFILES=${PROFILES}" tool/profiles/profile_cppstd11"
PROFILES=${PROFILES}" tool/profiles/profile_cppstd11_noexceptions"
PROFILES=${PROFILES}" tool/profiles/profile_cppstd17"
PROFILES=${PROFILES}" tool/profiles/profile_cppstd17_noexceptions"
PROFILES_STD98=${PROFILES_STD98}" tool/profiles/profile_cppstd98"
PROFILES_STD98=${PROFILES_STD98}" tool/profiles/profile_cppstd98_noexceptions"
BUILD_TYPE="Debug Release"

for pr in ${PROFILES} ${PROFILES_STD98}; do
	for type in ${BUILD_TYPE}; do
		conan build mechanism/test --build=missing -pr ${pr} -s build_type=${type}
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
		fi
	done
done

PLATFORM_OS=${PLATFORM_OS}" POSIX"
PLATFORM_OS=${PLATFORM_OS}" STDCPP"
for pr in ${PROFILES}; do
	for type in ${BUILD_TYPE}; do
		for platform in ${PLATFORM_OS}; do
			conan build platform/test --build=missing -pr ${pr} -s build_type=${type} -o platform_os=${platform}
			if [ "$?" != "0" ]; then
				EXIT_CODE=1
			fi
		done
	done
done

exit ${EXIT_CODE}
