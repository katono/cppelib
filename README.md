# CppELib

CppELib is a portable C++ library for embedded software development.
This works on the freestanding environment that provides few standard C++ libraries.

[![Build Status](https://app.travis-ci.com/katono/cppelib.svg?branch=master)](https://app.travis-ci.com/katono/cppelib)
[![Build status](https://ci.appveyor.com/api/projects/status/v2kqt2jvw8m7mija/branch/master?svg=true)](https://ci.appveyor.com/project/katono/cppelib/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/katono/cppelib/badge.svg?branch=master)](https://coveralls.io/github/katono/cppelib?branch=master)


## Description

CppELib provides these mechanisms:

* Assertion

    This is used instead of standard `assert()` macro.
    If an assertion fails, it throws C++ exception.
    If this exception is thrown, you must do shutdown your application safely.
    There are two assertion macros.
    One is always enabled, another is enabled only when `NDEBUG` macro is not defined.

* Container

    STL-like containers without dynamic memory allocation: fixed capacity vector, fixed capacity deque, intrusive list, etc.

* OSWrapper

    If you use a vendor's real-time operating system (RTOS), perhaps you can't use common class library like `std::thread`.
    OSWrapper provides abstract C++ interface of common RTOS: thread, mutex, event flag, message queue, memory pool, etc.
    This makes your application more portable and testable.
    And you can get a simulation environment for debug or test of your application on your host machine without the target board too (but real-time property may not be guaranteed).
    If the source files for your OS platform are not prepared in `platform` directory, you need to implement concrete classes of OS API wrapper. These concrete classes are only used for Dependency Injection as the application is initialized.

## Repository

You can get the latest version from here.

https://github.com/katono/cppelib


## Requirements

* `mechanism`: C++98 or later (freestanding)
* `platform` of Windows and POSIX: C++11 or later
* C++ Exception enabled


## Installation

Add source and header files to your application source tree and add `mechanism` and `platform` directories to the include path.


## Test

Tests of CppELib use [CppUTest](http://cpputest.github.io/) because this testing framework is portable.
Tests can be run on multi-platform: Visual Studio, g++, and your target platform.


## License

MIT License


