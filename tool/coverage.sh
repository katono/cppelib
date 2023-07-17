#!/bin/sh

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
STD_CPP="11 17"
for dir in ${TEST_DIRS}; do
	for std in ${STD_CPP}; do
		conan build ${dir} --build=missing -s compiler.cppstd=${std} -c tools.build:cxxflags+="--coverage" -c tools.build:exelinkflags+="--coverage" -of ${COVERAGE_BUILD_DIR}/${dir} -s build_type=Debug
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
		fi
	done
done

conan build mechanism/test --build=missing -s compiler.cppstd=98 -c tools.build:cxxflags+="--coverage" -c tools.build:exelinkflags+="--coverage" -of ${COVERAGE_BUILD_DIR}/${dir} -s build_type=Debug
if [ "$?" != "0" ]; then
	EXIT_CODE=1
fi

lcov -d ${COVERAGE_BUILD_DIR} -c -o ${COVERAGE_BUILD_DIR}/coverage.info
lcov -r ${COVERAGE_BUILD_DIR}/coverage.info "*/CppUTest*/*" "/usr/*" "*/test/*" -o ${COVERAGE_BUILD_DIR}/coverage.info
genhtml -o ${COVERAGE_BUILD_DIR}/coverage_report --num-spaces 4 -s --legend ${COVERAGE_BUILD_DIR}/coverage.info

exit ${EXIT_CODE}
