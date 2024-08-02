#!/bin/bash

if [ "$1" = "" ]; then
	echo "Usage: $0 REPO_ROOT"
	exit
fi

REPO_ROOT=$1
cd ${REPO_ROOT}

CPPCHECK_BUILD_DIR="cppcheck-build-dir"
mkdir -p ${CPPCHECK_BUILD_DIR}

cppcheck \
mechanism/Assertion/ \
mechanism/Container/ \
mechanism/OSWrapper/ \
platform/ItronOSWrapper/ \
platform/PosixOSWrapper/ \
platform/StdCppOSWrapper/ \
platform/WindowsOSWrapper/ \
platform/TestDoubleOSWrapper/ \
mechanism/test/StaticAnalysis/StaticAnalysis.cpp \
--cppcheck-build-dir=${CPPCHECK_BUILD_DIR} \
--enable=style \
-I mechanism/ \
-I platform/ \
--inline-suppr \
--suppress=incorrectStringBooleanError \
-j 8 \
--language=c++ \
--std=c++03 \
--error-exitcode=-1

