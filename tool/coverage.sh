#!/bin/bash

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

COVERAGE_BUILD_DIR=coverage_build
rm -rf ${COVERAGE_BUILD_DIR}

EXIT_CODE=0

PROFILES=${PROFILES}" tool/profiles/profile_cppstd11_coverage"
PROFILES=${PROFILES}" tool/profiles/profile_cppstd11_coverage_noexceptions"
PROFILES=${PROFILES}" tool/profiles/profile_cppstd17_coverage"
PROFILES=${PROFILES}" tool/profiles/profile_cppstd17_coverage_noexceptions"
PROFILES_STD98=${PROFILES_STD98}" tool/profiles/profile_cppstd98_coverage"
PROFILES_STD98=${PROFILES_STD98}" tool/profiles/profile_cppstd98_coverage_noexceptions"

for pr in ${PROFILES} ${PROFILES_STD98}; do
	conan build mechanism/test --build=missing -pr ${pr} -of ${COVERAGE_BUILD_DIR}/${dir} -s build_type=Debug
	if [ "$?" != "0" ]; then
		EXIT_CODE=1
	fi
done

PLATFORM_OS=${PLATFORM_OS}" POSIX"
PLATFORM_OS=${PLATFORM_OS}" STDCPP"
for pr in ${PROFILES}; do
	for platform in ${PLATFORM_OS}; do
		conan build platform/test --build=missing -pr ${pr} -of ${COVERAGE_BUILD_DIR}/${dir} -s build_type=Debug -o platform_os=${platform}
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
		fi
	done
done

lcov -d ${COVERAGE_BUILD_DIR} -c -o ${COVERAGE_BUILD_DIR}/coverage.info
lcov -r ${COVERAGE_BUILD_DIR}/coverage.info "*/CppUTest*/*" "/usr/*" "*/test/*" -o ${COVERAGE_BUILD_DIR}/coverage.info
genhtml -o ${COVERAGE_BUILD_DIR}/coverage_report --num-spaces 4 -s --legend ${COVERAGE_BUILD_DIR}/coverage.info

exit ${EXIT_CODE}
