# CppELib

CppELib is a portable C++ library for embedded software development.
This works on the freestanding environment such as:
* No standard C++ libraries
* Disabled exception-handling

[![CI](https://github.com/katono/cppelib/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/katono/cppelib/actions/workflows/main.yml)
[![Build status](https://ci.appveyor.com/api/projects/status/v2kqt2jvw8m7mija/branch/master?svg=true)](https://ci.appveyor.com/project/katono/cppelib/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/katono/cppelib/badge.svg?branch=master)](https://coveralls.io/github/katono/cppelib?branch=master)


## Description

CppELib provides these mechanisms:

* Assertion

    This is used instead of standard `assert()` macro.
    If an assertion fails, you must handle to do shutdown your application safely.
    There are two assertion macros.
    CHECK_ASSERT() is always enabled, DEBUG_ASSERT() is enabled only when `NDEBUG` macro is not defined.

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


## Documentation

https://katono.github.io/cppelib-docs


## Requirements

* `mechanism`: C++98 or later (freestanding)
* `platform` of Windows and POSIX: C++11 or later


## Installation

Add source and header files to your application source tree and add `mechanism` and `platform` directories to the include path.

If you can use [Conan](https://conan.io/) C++ package manager, execute these commands in this repo root directory to install `cppelib_mechanism` package and `cppelib_platform` package.
```
conan create mechanism
conan create platform
```

If you want these packages with C++ exception disabled, you can use `tool/profiles/profile_noexceptions` to include in your conan profile.
In that case, all your packages must use the same conan profile.
See `tool/build_package.sh` to know about how to use a sample conan profile.


## Test

Tests of CppELib use [CppUTest](http://cpputest.github.io/) because this testing framework is portable.
Tests can be run on multi-platform: Visual Studio, g++, and your target platform.

Execute these conan commands in this repo root directory to run tests.

```
conan build mechanism/test --build=missing
conan build platform/test --build=missing
```


## License

MIT License


