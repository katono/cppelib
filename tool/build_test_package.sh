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
PLATFORM_OS=${PLATFORM_OS}" POSIX"
PLATFORM_OS=${PLATFORM_OS}" STDCPP"
for pr in ${PROFILES}; do
	conan create mechanism/test/package --build=missing -pr ${pr}
	if [ "$?" != "0" ]; then
		EXIT_CODE=1
	fi
	for platform in ${PLATFORM_OS}; do
		conan create platform/test/package --build=missing -pr ${pr} -o platform_os=${platform}
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
		fi
	done
done

exit ${EXIT_CODE}
