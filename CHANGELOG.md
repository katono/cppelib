# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.7.0] - 2025-01-04

### Added

- Added methods in `OSWrapper::FixedMemoryPool`
  - OSWrapper::FixedMemoryPool::allocateMemory
  - OSWrapper::FixedMemoryPool::tryAllocateMemory
  - OSWrapper::FixedMemoryPool::timedAllocateMemory
  - OSWrapper::FixedMemoryPool::getNumberOfAvailableBlocks
  - OSWrapper::FixedMemoryPool::getMaxNumberOfBlocks

### Changed

- Added cppcheck-suppress comments

## [1.6.0] - 2024-08-04

### Added

- Added Test Double implementation of `OSWrapper`
- Added cppcheck script to do static analysis
- Added static analysis in CI
- Added deployment of Doxygen docs in CI
- Added helper class for test to run the test code on a target environment
- Added conan profiles for test
- Added Changelog

### Changed

- Changed to include `LICENSE` file in conan package

## [1.5.0] - 2024-08-02

### Added

- Added `conanfile.py` and `CMakeLists.txt` to build, test, and packaging libraries by Conan + CMake
- Add `CPPELIB_NO_EXCEPTIONS` macro to be able to build without C++ exception
- Add `AssertHandler` interface in `Assertion` for handling assertion failure
- Add method: `Assertion::setHandler()`
- Added CI by GitHub Actions
- Added build package script
- Added clean build directory script

### Fixed

- Fixed bug for 8bit, 16bit, 64bit by zero extension on `BitPattern`

### Changed

- Reduced ROM size of string literal with `Assertion`
- Changed run test and coverage script to use Conan

### Removed

- Removed method: `OSWrapper::Thread::exit()`
- Removed CI by Travis
- Removed Visual Studio test projects (because using Conan)
- Removed CppUTest submodule (because using Conan)

## [1.4.2] - 2021-10-17

### Added

- Added method: `OSWrapper::ThreadPool::getThreadName()`
- Added copy-constructor of `OSWrapper::Timeout` explicitly

### Changed

- Changed to be visible thread name on debugging

## [1.4.1] - 2021-04-08

### Fixed

- Fixed issue of compile error of `Container` on Visual Studio with language standard C++17

## [1.4.0] - 2020-01-12

### Added

- Added `ThreadPool` in `OSWrapper`
- Added method: `OSWrapper::Thread::getPriorityHigherThan()`

## [1.3.1] - 2019-05-20

### Added

- Added run test script
- Added coverage script
- Added CI by Travis, Appveyor, Coveralls

## [1.3.0] - 2019-05-05

### Added

- Added `PreallocatedVector` in `Container`
- Added `PreallocatedDeque` in `Container`
- Added `Doxyfile`

### Changed

- Changed to add Doxygen comments overall

## [1.2.1] - 2019-04-20

### Fixed

- Fixed for static analysis

## [1.2.0] - 2019-04-19

### Added

- Added `OneShotTimer` in `OSWrapper`

## [1.1.0] - 2019-04-14

### Added

- Added POSIX implementation of `OSWrapper`

## [1.0.1] - 2019-04-07

### Added

- Added Standard C++ implementation of `OSWrapper`

## [1.0.0] - 2018-07-26

### Added

- Added assertion macros in `Assertion`
- Added `FixedVector` in `Container`
- Added `FixedDeque` in `Container`
- Added `IntrusiveList` in `Container`
- Added `Array` in `Container`
- Added `BitPattern` in `Container`
- Added `Thread` in `OSWrapper`
- Added `Mutex` in `OSWrapper`
- Added `EventFlag` in `OSWrapper`
- Added `MessageQueue` in `OSWrapper`
- Added `FixedMemoryPool` in `OSWrapper`
- Added `VariableMemoryPool` in `OSWrapper`
- Added `PeriodicTimer` in `OSWrapper`
- Added Windows implementation of `OSWrapper`
- Added ITRON implementation of `OSWrapper`

[1.7.0]: https://github.com/katono/cppelib/compare/1.6.0...1.7.0
[1.6.0]: https://github.com/katono/cppelib/compare/1.5.0...1.6.0
[1.5.0]: https://github.com/katono/cppelib/compare/1.4.2...1.5.0
[1.4.2]: https://github.com/katono/cppelib/compare/1.4.1...1.4.2
[1.4.1]: https://github.com/katono/cppelib/compare/1.4.0...1.4.1
[1.4.0]: https://github.com/katono/cppelib/compare/1.3.1...1.4.0
[1.3.1]: https://github.com/katono/cppelib/compare/1.3.0...1.3.1
[1.3.0]: https://github.com/katono/cppelib/compare/1.2.1...1.3.0
[1.2.1]: https://github.com/katono/cppelib/compare/1.2.0...1.2.1
[1.2.0]: https://github.com/katono/cppelib/compare/1.1.0...1.2.0
[1.1.0]: https://github.com/katono/cppelib/compare/1.0.1...1.1.0
[1.0.1]: https://github.com/katono/cppelib/compare/1.0.0...1.0.1
[1.0.0]: https://github.com/katono/cppelib/releases/tag/1.0.0
