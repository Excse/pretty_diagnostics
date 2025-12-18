# Pretty Diagnostics

Pretty Diagnostics is a small C++ library for rendering elegant, informative diagnostics with labeled spans across one or 
multiple files similar to the output you get from Rust compilers. It is useful for compilers, interpreters, code analyzers,
and any tool that needs to point to precise locations in text sources and explain what went wrong.

![State](https://img.shields.io/github/actions/workflow/status/Excse/pretty_diagnostics/cmake-single-platform.yml?style=flat&label=Build%2FTest)
![Last Commit](https://img.shields.io/github/last-commit/Excse/pretty_diagnostics?style=flat&label=Last%20Commit)
![Contributors](https://img.shields.io/github/contributors/Excse/pretty_diagnostics?style=flat&label=Contributors)
![Visitors](https://api.visitorbadge.io/api/visitors?path=https%3A%2F%2Fgithub.com%2FExcse%2Fpretty_diagnostics&label=Repository%20Visits&countColor=%232ccce4&style=flat&labelStyle=none)
![License](https://img.shields.io/github/license/Excse/pretty_diagnostics?style=flat&label=License)
![Stars](https://img.shields.io/github/stars/Excse/pretty_diagnostics)

## Table of Contents

- [Features](#features)
- [Demo](#demo)
- [Requirements](#requirements)
- [Installation](#installation)
  - [Option A: Install to your system](#option-a-install-to-your-system)
  - [Option B: FetchContent (vendored)](#option-b-fetchcontent-vendored)
- [CMake Integration](#cmake-integration)
- [Example Usage](#example-usage)
- [License](#license)

## Features

- Labeled diagnostics with precise spans
- Grouping by file and line with clean text layout
- Output similar to modern compilers (multi-line, guides/arrows)
- Works with multiple sources/files in a single report

## Demo

The text renderer produces output like this:

![Rendered example](resources/example.png)

## Requirements

- C++20-compatible compiler
- CMake 4.2+ (for the provided configuration)
- Tested on Linux; expected to work on macOS and Windows as well

## Installation

Clone the repository:

```sh
git clone https://github.com/Excse/pretty_diagnostics.git
cd pretty_diagnostics
```

### Option A: Install to your system

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
                    -DBUILD_TESTING=OFF        \
                    -DBUILD_EXECUTABLE=OFF     \
                    -DMAKE_INSTALLABLE=ON      \
                    -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Release
sudo cmake --install build --config Release
```

### Option B: FetchContent (vendored)

Add the library as a dependency without installing it globally:

```cmake
include(FetchContent)

FetchContent_Declare(
  pretty_diagnostics
  GIT_REPOSITORY https://github.com/Excse/pretty_diagnostics.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(pretty_diagnostics)
```

## CMake Integration

After installing or making it available via `FetchContent`, link the target to your project:

```cmake
find_package(pretty_diagnostics CONFIG REQUIRED) # not needed when using FetchContent with exported targets in the same build

target_link_libraries(YourTarget PRIVATE pretty_diagnostics::pretty_diagnostics)
```

## Example Usage

```cpp
#include <sstream>
#include <memory>

#include "pretty_diagnostics/report.hpp"
#include "pretty_diagnostics/source.hpp"

using namespace pretty_diagnostics;

int main() {
    auto file = std::make_shared<FileSource>("resources/main.c");

    const auto report = Report::Builder()
                       .severity(Severity::Error)
                       .message("Displaying a brief summary of what happened")
                       .code("E1337")
                       .label("And this is the function that actually makes the magic happen", { file_source, 37, 43 })
                       .label("This is the string that is getting printed to the console", { file_source, 44, 60 })
                       .label("Relevant include to enable the usage of printf", { file_source, 10, 17 })
                       .label("This is a new line", { file_source, 1, 0, 1, 1 })
                       .note("This example showcases every little detail of the library, also with the capability of line wrapping.")
                       .note("Visit https://github.com/Excse/pretty_diagnostics for more help.")
                       .build();

    auto renderer = TextRenderer(report);
    auto stream = std::ostringstream();
    report.render(renderer, stream);
    
    // print or log the result
    std::cout << stream.str();
}
```

## License

This project is licensed under the MIT License, see the [LICENSE](LICENSE.txt) file for details.
