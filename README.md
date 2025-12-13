# Pretty Diagnostics

Pretty Diagnostics is a library designed to produce elegant and informative error messages, with labeled references to code segments across multiple files, similar to the error output in Rust. It is particularly useful for compilers, interpreters, and any program that needs to provide meaningful error messages based on text files. The goal is to make error diagnostics as user-friendly and descriptive as possible, helping users quickly identify and resolve issues in their code.

![State](https://img.shields.io/github/actions/workflow/status/Excse/pretty_diagnostics/cmake-single-platform.yml?style=flat&label=Build%2FTest)
![Last Commit](https://img.shields.io/github/last-commit/Excse/pretty_diagnostics?style=flat&label=Last%20Commit)
![Contributors](https://img.shields.io/github/contributors/Excse/pretty_diagnostics?style=flat&label=Contributors)
![Visitors](https://api.visitorbadge.io/api/visitors?path=https%3A%2F%2Fgithub.com%2FExcse%2Fpretty_diagnostics&label=Repository%20Visits&countColor=%232ccce4&style=flat&labelStyle=none)
![License](https://img.shields.io/github/license/Excse/pretty_diagnostics?style=flat&label=License)
![Stars](https://img.shields.io/github/stars/Excse/pretty_diagnostics)

## Features

- Generate structured, labeled error messages.
- Highlight code segments across multiple files.
- Enhance the debugging experience for developers.
- Focused on compilers, interpreters, and other text file-based error reporting systems.

## Installation

Download the project or use git to clone it:
```sh
$ git clone https://github.com/Excse/pretty_diagnostics.git
```

Navigate to the directory where the project is located and execute these commands:
```sh
$ cmake -S . -B own_build Release -DCMAKE_BUILD_TYPE=Release \
                                  -DBUILD_TESTING=OFF        \
                                  -DBUILD_EXECUTABLE=OFF     \
                                  -DMAKE_INSTALLABLE=ON      \
                                  -DBUILD_SHARED_LIBS=OFF
$ cmake --build own_build --config Release
$ sudo cmake --install own_build --config Release
```

When you are finished, you are ready to use the library. If you want to use it in a
cmake project, use the following code to implement it.

and in CMakeLists.txt:
```cmake
find_package(pretty_diagnostics CONFIG REQUIRED)

target_link_libraries(!!PROJECT_NAME!! PRIVATE pretty_diagnostics::pretty_diagnostics)
```

## Example Usage

```cpp
const auto file = std::make_shared<FileSource>("resources/main.c");

const auto report = Report::Builder()
        .severity(Severity::Error)
        .message("Displaying a brief summary of what happened")
        .code("E1337")
        .label("And this is the function that actually makes the magic happen", {file, 37, 43})
        .label("This is the string that is getting printed to the console", {file, 44, 60})
        .label("Relevant include to enable the usage of printf", {file, 10, 17})
        .build();

auto renderer = TextRenderer(report);
auto stream = std::ostringstream();
report.render(renderer, stream);
```

The output looks like:</br>
![Image of this example](resources/example.png)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
