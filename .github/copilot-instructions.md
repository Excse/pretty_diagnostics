# Copilot Instructions for Pretty Diagnostics

## Repository Overview

Pretty Diagnostics is a **C++ library** for producing elegant error messages with labeled references to code segments across multiple files, similar to Rust's error output. It's designed for compilers, interpreters, and programs that need meaningful error reporting from text files.

**Key Details:**
- **Language**: C++23 (required, enforced)
- **Build System**: CMake (minimum 3.20)
- **Testing**: GoogleTest framework with snapshot testing
- **Size**: ~1,679 lines of code across 67 source files
- **License**: MIT (main) / BSD 3-Clause (tests)

## Build and Validation Commands

**ALWAYS follow these exact command sequences. They have been validated to work correctly.**

### Clean Build Process
```bash
# 1. Configure (ALWAYS run first)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DMAKE_INSTALLABLE=ON -DBUILD_SHARED_LIBS=ON

# 2. Build (takes ~30 seconds)
cmake --build build --config Release

# 3. Run Tests (REQUIRED before any code changes)
cd build/tests && ./pretty_diagnostics_tests --update_snapshots
cd ../..  # Return to root
```

### Test-Only Commands
```bash
# Run tests without snapshot updates
cd build/tests && ./pretty_diagnostics_tests
cd ../..

# Update test snapshots (use when test expectations change)
cd build/tests && ./pretty_diagnostics_tests --update_snapshots
cd ../..
```

### Installation Testing
```bash
# Test installation (validates library packaging)
cmake --install build --prefix /tmp/test_install
```

### Environment Requirements
- **CMake**: 3.20+ (current: 3.31.6)
- **GCC**: Supports C++23 (current: 13.3.0)
- **Build time**: ~30 seconds clean build
- **Test time**: <1 second (13 tests)

### Critical Build Notes
- **ALWAYS clean before major changes**: `rm -rf build` then reconfigure
- **NEVER commit build artifacts**: The `build/` directory is gitignored
- Tests use snapshot testing - run with `--update_snapshots` when output format changes
- GoogleTest is fetched automatically via CMake FetchContent

## Project Architecture and Layout

### Core Library Structure
```
include/pretty_diagnostics/    # Public API headers
├── report.h                   # Main Report class and severity enums
├── renderer.h                 # TextRenderer for output formatting  
├── source.h                   # FileSource for reading source files
├── span.h                     # Text span utilities
└── label.h                    # Label annotations

src/                          # Implementation files
├── report.cpp                # Report building and management
├── renderer.cpp              # Text rendering logic (has TODO)
├── source.cpp                # File source handling
├── span.cpp                  # Span calculations
└── label.cpp                 # Label processing
```

### Key Classes and APIs
- **`Report`**: Main diagnostic report with Builder pattern
- **`TextRenderer`**: Renders reports to text streams
- **`FileSource`**: Represents source files for diagnostics
- **`Label`**: Code segment annotations with messages
- **`Severity`**: Error/Warning/Info/Unknown levels

### Test Infrastructure
```
tests/
├── CMakeLists.txt            # Test configuration with GoogleTest
├── main.cpp                  # Test runner with --update_snapshots support
├── pretty_diagnostics/       # Unit tests for each component
├── utils/                    # Test utilities (snapshot.h)
└── snapshots/                # Expected test outputs (auto-generated)
```

### Configuration Files
- **`CMakeLists.txt`**: Main build configuration with options
- **`cmake/Config.cmake.in`**: Package configuration template
- **`.gitignore`**: Standard C++/CMake exclusions + build/
- **No code formatting tools configured** (no .clang-format)

### CI/CD Pipeline
**GitHub Actions**: `.github/workflows/cmake-single-platform.yml`
- Runs on Ubuntu latest
- Full build → test → install validation
- Uses same command sequence as above

## Making Changes

### Before Modifying Code
1. **Always build and test first** to establish baseline
2. **Check for existing tests** in `tests/pretty_diagnostics/`
3. **Identify relevant source files** using the structure above

### Code Modification Guidelines
- **C++23 standard required** - use modern C++ features
- **Follow existing patterns** in header/implementation organization
- **Update tests** if changing public APIs or output formats
- **Run tests frequently** during development

### Common Change Locations
- **Add new diagnostic features**: `include/pretty_diagnostics/report.h` and `src/report.cpp`
- **Modify output formatting**: `include/pretty_diagnostics/renderer.h` and `src/renderer.cpp`
- **Change file handling**: `include/pretty_diagnostics/source.h` and `src/source.cpp`
- **Add new tests**: `tests/pretty_diagnostics/<component>.cpp`

### Known Issues to Be Aware Of
- **TODO in renderer.cpp**: `MAX_TERMINAL_WIDTH` hardcoded to 80, could be made dynamic
- **License inconsistency**: MIT in README vs BSD in test files
- **No linting configured**: No automated code style enforcement

### Validation Steps
1. **Build succeeds**: `cmake --build build`
2. **Tests pass**: All 13 tests in 4 suites should pass
3. **Installation works**: `cmake --install build --prefix /tmp/test`
4. **No new warnings**: Check build output for compiler warnings

## Quick Reference

### Repository Root Files
- `CMakeLists.txt` - Main build configuration
- `README.md` - Project documentation with usage examples
- `LICENSE.txt` - MIT license
- `.gitignore` - Build artifacts exclusion

### Essential Commands Summary
```bash
# Full clean build and test
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build
cd build/tests && ./pretty_diagnostics_tests --update_snapshots

# Quick test after changes
cmake --build build && cd build/tests && ./pretty_diagnostics_tests
```

**Trust these instructions first.** Only explore further if information is incomplete or errors are encountered. The commands and structure documented here have been validated to work correctly.