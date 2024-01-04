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

TEST_DIRS=${TEST_DIRS}" mechanism/test"
TEST_DIRS=${TEST_DIRS}" platform/test"
PROFILES=${PROFILES}" tool/profile_cppstd11_coverage"
PROFILES=${PROFILES}" tool/profile_cppstd11_coverage_noexceptions"
PROFILES=${PROFILES}" tool/profile_cppstd17_coverage"
PROFILES=${PROFILES}" tool/profile_cppstd17_coverage_noexceptions"
PROFILES=${PROFILES}" tool/profile_cppstd98_coverage"
PROFILES=${PROFILES}" tool/profile_cppstd98_coverage_noexceptions"
STD_CPP="11 17"
for dir in ${TEST_DIRS}; do
	for pr in ${PROFILES}; do
		if [[ ${dir} =~ platform ]] && [[ ${pr} =~ cppstd98 ]]; then
			continue
		fi
		conan build ${dir} --build=missing -pr ${pr} -of ${COVERAGE_BUILD_DIR}/${dir} -s build_type=Debug
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
		fi
	done
done

lcov -d ${COVERAGE_BUILD_DIR} -c -o ${COVERAGE_BUILD_DIR}/coverage.info
lcov -r ${COVERAGE_BUILD_DIR}/coverage.info "*/CppUTest*/*" "/usr/*" "*/test/*" -o ${COVERAGE_BUILD_DIR}/coverage.info
genhtml -o ${COVERAGE_BUILD_DIR}/coverage_report --num-spaces 4 -s --legend ${COVERAGE_BUILD_DIR}/coverage.info

exit ${EXIT_CODE}
