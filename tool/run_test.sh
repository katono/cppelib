#!/bin/sh

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

EXIT_CODE=0

TEST_DIRS=${TEST_DIRS}" mechanism/test"
TEST_DIRS=${TEST_DIRS}" platform/test"
STD_CPP="11 17"
for dir in ${TEST_DIRS}; do
	for std in ${STD_CPP}; do
		conan build ${dir} --build=missing -s compiler.cppstd=${std}
		if [ "$?" != "0" ]; then
			EXIT_CODE=1
		fi
	done
done

conan build mechanism/test --build=missing -s compiler.cppstd=98
if [ "$?" != "0" ]; then
	EXIT_CODE=1
fi

exit ${EXIT_CODE}
